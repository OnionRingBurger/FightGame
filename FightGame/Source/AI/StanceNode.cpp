#include "AI/StanceNode.h"

#include <algorithm>
#include <cmath>

#include "MathAssist.h"
#include "DebugConsole.h"

// 不利脱出、優位維持、様子見ノード
StanceNode::StanceNode(
	std::unique_ptr<Node> escapeDisadvantage,
	std::unique_ptr<Node> holdAdvantage,
	std::unique_ptr<Node> neutralProbe)
	: CompositeNode()
{
	// 各ノードを追加する
	AddNode(std::move(escapeDisadvantage));
	AddNode(std::move(holdAdvantage));
	AddNode(std::move(neutralProbe));
}

StanceNode::~StanceNode()
{
}

Node::Status StanceNode::Tick(AIContext& context, const AIBlackboard& blackBoard, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result)
{

	// 行われたアクション等に応じて方針更新値を変更する
	mind.updatePoints = GetNewUpdatePoint(blackBoard, mind, systemInfo);
	
	// 方針更新値が一定以上になった場合方針を更新し、mindを更新する
	if (mind.updatePoints >= mind.maxUpdatePoints)
	{
		AIStance oldStance = mind.stance;
		mind.stance = GetNewStance(context, blackBoard, mind);
		// 連続数を更新
		if (oldStance == mind.stance) mind.stanceContinueCount++;
		else mind.stanceContinueCount = 0;
		std::cout << mind.stanceContinueCount << std::endl;
		mind.updatePoints = 0.0f;
		mind.attackMissCount = 0;
		mind.attackSuccessCount = 0;
		mind.sameAttackStreak = 0;
	}

	// 方針が存在しない場合失敗を返す
	if (mind.stance == AIStance::NoneStance) return NODE_FAILURE;

	// 対応するノードが存在しなかった場合失敗を返す
	const int stanceIndex = static_cast<int>(mind.stance);
	if (stanceIndex < 0 || stanceIndex >= static_cast<int>(childNodes.size()))
	{
		return NODE_FAILURE;
	}

	// コンテキストを更新する
	bool isSafe = UpdateContext(context, stanceIndex);
	if (!isSafe) return NODE_FAILURE;

	// 対応する方針のノードの更新を行い結果を返す
	return childNodes.at(stanceIndex)->Tick(context, blackBoard, mind, systemInfo, result);
}

float StanceNode::GetNewUpdatePoint(const AIBlackboard& blackBoard, const AIMind& mind, const AISystemInfo& systemInfo)
{
	// ここに諸々の増減を書いていく
	float points = mind.updatePoints;
	// 時間経過による増加
	points += systemInfo.tickDelta;
	// TODO 他にも書く

	return points;

}

// 各数値と重みから新たなStanceを割り出す（既存カーブのみ使用）
AIStance StanceNode::GetNewStance(const AIContext& context, const AIBlackboard& blackBoard, const AIMind& mind)
{
	const auto enemyIt = blackBoard.enemyDatas.find(context.agentEntity);
	if (enemyIt == blackBoard.enemyDatas.end())
	{
		return AIStance::NoneStance;
	}

	const AIBlackboard::BBEnemyData& enemyData = enemyIt->second;

	// ユーザーとの距離
	const float2 toPlayer(
		blackBoard.playerPos.x - enemyData.position.x,
		blackBoard.playerPos.y - enemyData.position.y);
	const float playerDistance = GetLength(toPlayer);

	// 得意間合い: attackDatas[0] の中点（無ければ 1）
	float preferredMid = 1.0f;
	if (!enemyData.attackDatas.empty())
	{
		const AIBlackboard::BBAttackData& atk = enemyData.attackDatas[0];
		preferredMid = atk.maxLength * 1.3f;
	}

	// Evidence 未配線のため仮値（0=被弾なし、成功も未計測）
	const int hitTakenCount = mind.sameAttackStreak;
	const int attackSuccessCount = mind.attackSuccessCount;

	// 各係数を作成
	const float distCoeff = enemyData.distCoefficient > 0.0f ? enemyData.distCoefficient : 1.0f;
	const float farnessCoeff = enemyData.farnessCoefficient > 0.0f ? enemyData.farnessCoefficient : 1.0f;
	const float hitCoeff = enemyData.hitCoefficient > 0.0f ? enemyData.hitCoefficient : 3.0f;
	const float stanceCoeff = enemyData.stanceCoefficient;

	// 有利位置: 得意距離に近いほど高い
	const float advantagePos = ScoreByDistance(playerDistance, preferredMid, distCoeff);
	// 不利位置: 得意距離から外れるほど高い
	const float disadvantagePos = 1.0f - advantagePos;
	// 距離の遠さ: 原点から離れるほど高い
	const float farness = 1.0f - ScoreByDistance(playerDistance, 0.0f, farnessCoeff);

	// 被弾の少なさ / 多さ
	const float HitScore = ScoreByHitCount(hitTakenCount, hitCoeff);
	const float manyHits = ApplyScoreBase(HitScore, 0.5f);
	const float fewHits = ApplyScoreBase(1.0f - HitScore, 0.5f);

	// 攻撃成功の多さ（未配線のため常に ScoreByHitCount(0)=1 寄り）
	const float manySuccess = 1.0f - ScoreByHitCount(attackSuccessCount, hitCoeff);



	// 各要素をかけてスコアを算出
	// 様子見: 被弾の少なさ * 距離の遠さ
	float scoreProbe = fewHits * farness;
	 // 継続戦闘(有利維持): 成功の多さ * 有利位置
	float scoreHold = manySuccess * advantagePos;
	// 戦闘回避(不利脱出): 不利位置 * 被弾の多さ
	float scoreEscape = disadvantagePos * manyHits;


	DebugConsole::SetDrawPos(15, 12);

	// ステート継続を現行方針のスコアにだけ掛ける
	const float stanceContinue = ScoreByStance(mind.stanceContinueCount, stanceCoeff);
	AIStance oldStance = mind.stance;
	switch (mind.stance)
	{
	case AIStance::NeutralProbe:
		scoreProbe *= stanceContinue;
		break;
	case AIStance::HoldAdvantage:
		scoreHold *= stanceContinue;
		break;
	case AIStance::EscapeDisadvantage:
		scoreEscape *= stanceContinue;
		break;
	case AIStance::NoneStance:
	default:
		break;
	}

	std::cout << "様子見スコア: " << scoreProbe << std::endl;
	DebugConsole::SetDrawPos(15, 13);
	std::cout << "有利スコア: " << scoreHold << std::endl;
	DebugConsole::SetDrawPos(15, 14);
	std::cout << "不利スコア: " << scoreEscape << std::endl;
	DebugConsole::SetDrawPos(15, 15);
	std::cout << "得意間合いへの近さ: " << advantagePos << std::endl;

	// スタンスを決定
	AIStance newStance = AIStance::NeutralProbe;
	float best = scoreProbe;
	if (scoreHold > best)
	{
		best = scoreHold;
		newStance = AIStance::HoldAdvantage;
	}
	if (scoreEscape > best)
	{
		newStance = AIStance::EscapeDisadvantage;
	}

	return newStance;
}

float StanceNode::ApplyScoreBase(float score, float base)
{
	// 基準を基に正規化
	float normalizedScore = (score * (1.0f - base)) + base;
	// 基準値を掛ける
	return normalizedScore;
}

// 距離用カーブ、mid に近いほど値が大きくなる
float StanceNode::ScoreByDistance(float dist, float mid, float coefficient)
{
	if (coefficient <= 0.0f)
	{
		coefficient = 0.0001f;
	}
	const float targetDis = std::fabs(mid - dist);
	float normalizedScore = std::exp(-targetDis / coefficient);
	return normalizedScore;
}

// 被弾数カーブ、0.5を基準に値が増えるほど数値が大きくなる
float StanceNode::ScoreByHitCount(int hitCount, float coefficient)
{
	if (coefficient <= 0.0f)
	{
		coefficient = 0.0001f;
	}
	float score = 1.0f -std::exp(-static_cast<float>(hitCount) / coefficient);
	return score;
}


// 方針継続カーブ、回数に応じて coefficient で割っていく（coefficient>1 前提）
float StanceNode::ScoreByStance(int stanceCount, float coefficient)
{
	if (coefficient <= 1.0f)
	{
		coefficient = 1.0001f;
	}
	return min(1.0f / std::pow(coefficient, static_cast<float>(stanceCount)), 1.0f);
}
