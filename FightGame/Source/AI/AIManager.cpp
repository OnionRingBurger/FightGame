#include "AI/AIManager.h"
#include "DebugConsole.h"
#include "AI/RoleCurve.h"

AIManager::AIManager(float a_behaviorTickInterval)
	: behaviorTickInterval(a_behaviorTickInterval)
	, behaviorTickAccum(0.0f)
{
}

AIManager::~AIManager()
{
}

bool AIManager::RegisterTree(const std::string& aiType, std::unique_ptr<BehaviorTree> tree)
{
	// ツリーや登録要素が空だったら失敗
	if (!tree || aiType.empty())
	{
		return false;
	}
	// 登録
	behavioreTreeMap[aiType] = std::move(tree);
	return true;
}

bool AIManager::RegisterAI(Entity entity, const std::string& aiType)
{
	// 対応するビヘイビアツリーが存在しなかったら失敗
	if (behavioreTreeMap.find(aiType) == behavioreTreeMap.end())
	{
		return false;
	}

	// 空のRuntimeDataを追加
	AIRuntime runtime;
	runtime.context.BTType = aiType;
	runtime.context.agentEntity = entity;
	runtime.context.nodeIndexes.clear();
	runtime.context.actionDuration = 0.0f;
	runtime.result.MoveDir = float2(0.0f, 0.0f);
	runtime.result.UseAttack = false;
	runtime.result.AttackIndex = 0;

	runtimeMap[entity] = runtime;
	return true;
}

bool AIManager::UnregisterAI(Entity entity)
{
	// AIを削除
	const bool removed = runtimeMap.erase(entity) > 0;
	blackBoard.enemyDatas.erase(entity);
	return removed;
}

AIResult AIManager::ReadResult(Entity entity) const
{
	// 対象のEntityが存在する場合は
	const auto it = runtimeMap.find(entity);
	if (it == runtimeMap.end())
	{
		return AIResult();
	}
	return it->second.result;
}

void AIManager::WriteSenseFrame(const AISenseFrame& senseFrame)
{
	// プレイヤーの情報を更新
	blackBoard.playerPos = senseFrame.playerPos;
	blackBoard.isPlayerDead = senseFrame.isPlayerDead;

	// 
	for (const auto& pair : senseFrame.enemies)
	{
		// 書き込まれたデータの敵が未登録だった場合は抜ける
		if (runtimeMap.find(pair.first) == runtimeMap.end())
		{
			continue;
		}
		// データを更新する
		AIBlackboard::BBEnemyData& enemyData = blackBoard.enemyDatas[pair.first];
		enemyData.position = pair.second.position;
		enemyData.rotation = pair.second.rotation;
		enemyData.canMove = pair.second.canMove;
		enemyData.canAttack = pair.second.canAttack;
	}

	// 更新がなかったEntityを削除
	for (auto it = blackBoard.enemyDatas.begin(); it != blackBoard.enemyDatas.end(); )
	{
		if (senseFrame.enemies.find(it->first) == senseFrame.enemies.end())
		{
			it = blackBoard.enemyDatas.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void AIManager::WriteSenseProfile(const AISenseProfile& senseProfile)
{
	for (const auto& pair : senseProfile.enemies)
	{
		// 存在するか確認
		if (runtimeMap.find(pair.first) == runtimeMap.end())
		{
			continue;
		}

		AIBlackboard::BBEnemyData& enemyData = blackBoard.enemyDatas[pair.first];
		enemyData.attackDatas = pair.second.attackDatas;
		enemyData.distFocus = pair.second.distFocus;
		enemyData.farnessPref = pair.second.farnessPref;
		enemyData.hitSensitivity = pair.second.hitSensitivity;
		enemyData.stanceStickiness = pair.second.stanceStickiness;
		enemyData.probeContinuePowRate = pair.second.probeContinuePowRate;
		enemyData.holdContinuePowRate = pair.second.holdContinuePowRate;
		enemyData.escapeContinuePowRate = pair.second.escapeContinuePowRate;

		// !!!New!!!
		RoleCurveKnobs roleKnobs;
		roleKnobs.distFocus = enemyData.distFocus;
		roleKnobs.farnessPref = enemyData.farnessPref;
		roleKnobs.hitSensitivity = enemyData.hitSensitivity;
		roleKnobs.stanceStickiness = enemyData.stanceStickiness;
		const RoleCurveParams roleParams = MapRoleCurveKnobsToParams(roleKnobs);
		enemyData.distCoefficient = roleParams.distCoefficient;
		enemyData.farnessCoefficient = roleParams.farnessCoefficient;
		enemyData.hitCoefficient = roleParams.hitCoefficient;
		enemyData.stanceCoefficient = roleParams.stanceCoefficient;

		enemyData.profileReady = true;
	}
}

bool AIManager::IsRegisteredAI(Entity entity) const
{
	return runtimeMap.find(entity) != runtimeMap.end();
}

bool AIManager::IsSenseProfileReady(Entity entity) const
{
	const auto it = blackBoard.enemyDatas.find(entity);
	if (it == blackBoard.enemyDatas.end())
	{
		return false;
	}
	return it->second.profileReady;
}

void AIManager::WriteSystem(const AISystemInfo& a_systemInfo)
{
	// System情報を書き込む
	systemInfo = a_systemInfo;
}


// 行った攻撃終了結果を受け取る
void AIManager::NotifyAttackResolved(Entity attacker, int attackIndex, bool connected)
{

	// 攻撃者が登録されていなかったら抜ける
	const auto it = runtimeMap.find(attacker);
	if (it == runtimeMap.end())
	{
		return;
	}

	// 思考要素を更新
	AIMind& mind = it->second.mind;
	if (connected)
	{
		mind.attackSuccessCount++;
		if (mind.lastHitAttackIndex == attackIndex)
		{
			mind.sameAttackStreak++;
		}
		else
		{
			mind.lastHitAttackIndex = attackIndex;
			mind.sameAttackStreak = 1;
		}
	}
	else
	{
		mind.attackMissCount++;
		mind.sameAttackStreak = 0;
	}
}

// 攻撃的中時用
void AIManager::NotifyHitResolved(Entity hitEnemy, bool isGuard)
{
	auto it = runtimeMap.find(hitEnemy);
	if (it == runtimeMap.end())
	{
		return;
	}

	AIMind& mind = it->second.mind;

	mind.sameTakenCount++;
	if (isGuard)
	{
		mind.sameGuardCount++;
	}
}



void AIManager::Tick(float deltaTime)
{
	// 思考を更新
	// 更新頻度が各Entityの状態ごとに違うので
	// 各々が更新頻度を確認する
	TickAllAIMind(deltaTime);

	// BTの更新頻度を計算
	behaviorTickAccum += deltaTime;
	if (behaviorTickAccum >= behaviorTickInterval)
	{
		TickBehaviorTree(behaviorTickAccum);
		behaviorTickAccum = 0.0f;
	}
}

void AIManager::TickBehaviorTree(float behaviorDeltaTime)
{
	// 各Entityを更新
	for (auto& pair : runtimeMap)
	{
		AIRuntime& runtime = pair.second;

		// contextからビヘイビアツリーを取得
		const auto treeIt = behavioreTreeMap.find(runtime.context.BTType);
		// 対応するビヘイビアツリーが存在しなかったら次へ
		if (treeIt == behavioreTreeMap.end() || !treeIt->second)
		{
			continue;
		}

		runtime.result.MoveDir = float2(0.0f, 0.0f);
		runtime.result.UseAttack = false;
		runtime.result.UseGuard = false;
		runtime.result.AttackIndex = 0;

		// ビヘイビアツリーの更新を行う
		treeIt->second->Tick(
				runtime.context,
				blackBoard,
				runtime.mind,
				systemInfo,
				runtime.result,
				behaviorDeltaTime
			);
	}
}

void AIManager::TickAllAIMind(float deltaTime)
{
	// 各Entityを更新
	for (auto& pair : runtimeMap)
	{
		AIRuntime& runtime = pair.second;
		TickAIMind(
			runtime.context,
			blackBoard,
			runtime.mind,
			deltaTime,
			runtime.result
		);
	}
}


void AIManager::TickAIMind(AIContext& context, const AIBlackboard& blackBoard, AIMind& mind, float deltaTime, AIResult& result)
{
	// スタンス変更に至るポイントを更新
	UpdateStancePoint(mind, deltaTime);
	// 更新
	if (IsStanceUpdate(mind))
	{
		// スタンスの更新
		UpdateStance(
			context,
			blackBoard,
			mind,
			systemInfo,
			result
		);
	}
}

void AIManager::UpdateStancePoint(AIMind& mind, float deltaTime)
{
	// 現在のポイント
	float currentPoint = mind.updatePoints;
	// 時間経過による更新量
	float deltaPoint = deltaTime;
	// ガード数に応じてより早く更新する
	float guardPoint = mind.sameGuardCount * deltaTime * 1.5f;

	// ポイントの合計を出して更新
	float pointAmount = currentPoint + deltaPoint + guardPoint;
	mind.updatePoints = pointAmount;
}

void AIManager::UpdateStance(AIContext& context, const AIBlackboard& blackBoard, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result)
{
	// スタンスを更新
	AIStance oldStance = mind.stance;
	AIStance newStance = GetNewStance(context, blackBoard, mind);
	mind.stance = newStance;


	// スタンス連続数を更新
	if(oldStance == newStance)
	{
		mind.stanceContinueCount++;
	}
	else
	{
		mind.stanceContinueCount = 0;
		mind.sameTakenCount = 0;
		mind.sameGuardCount = 0;
	}

	// 1スタンス間のデータを初期化
	mind.updatePoints = 0.0f;
	mind.attackMissCount = 0;
	mind.attackSuccessCount = 0;
	mind.sameAttackStreak = 0;
}

bool AIManager::IsStanceUpdate(const AIMind& mind)
{
	return mind.updatePoints >= mind.maxUpdatePoints;
}


// 各数値と重みから新たなStanceを割り出す（既存カーブのみ使用）
AIStance AIManager::GetNewStance(const AIContext& context, const AIBlackboard& blackBoard, const AIMind& mind)
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
	const int hitTakenCount = mind.sameTakenCount - mind.sameGuardCount;
	const int attackSuccessCount = mind.attackSuccessCount;

    // 各係数を作成 !!!Old!!!
    // const float distCoeff = enemyData.distCoefficient > 0.0f ? enemyData.distCoefficient : 1.0f;
    // const float farnessCoeff = enemyData.farnessCoefficient > 0.0f ? enemyData.farnessCoefficient : 1.0f;
    // const float hitCoeff = enemyData.hitCoefficient > 0.0f ? enemyData.hitCoefficient : 3.0f;
    // const float stanceCoeff = enemyData.stanceCoefficient;

	// 各係数を作成 !!!Old!!!
	// RoleCurveKnobs roleKnobs;
	// roleKnobs.distFocus = enemyData.distFocus;
	// roleKnobs.farnessPref = enemyData.farnessPref;
	// roleKnobs.hitSensitivity = enemyData.hitSensitivity;
	// roleKnobs.stanceStickiness = enemyData.stanceStickiness;
	// const RoleCurveParams roleParams = MapRoleCurveKnobsToParams(roleKnobs);
	// const float distCoeff = roleParams.distCoefficient;
	// const float farnessCoeff = roleParams.farnessCoefficient;
	// const float hitCoeff = roleParams.hitCoefficient;
	// const float stanceCoeff = roleParams.stanceCoefficient;

	// !!!New!!!
	const float distCoeff = enemyData.distCoefficient;
	const float farnessCoeff = enemyData.farnessCoefficient;
	const float hitCoeff = enemyData.hitCoefficient;
	const float stanceCoeff = enemyData.stanceCoefficient;

	// 有利位置: 得意距離に近いほど高い
	const float advantagePos = ScoreByDistance(playerDistance, preferredMid, distCoeff);
	// 不利位置: 得意距離から外れるほど高い
	const float disadvantagePos = ApplyScoreBase((1.0f - advantagePos), 0.7f);
	// 距離の遠さ: 原点から離れるほど高い
	const float farness = 1.0f - ScoreByDistance(playerDistance, 0.0f, farnessCoeff);

	// 被弾の少なさ / 多さ
	const float HitScore = ScoreByHitCount(hitTakenCount, hitCoeff);
	const float manyHits = ApplyScoreBase(HitScore, 0.3f);
	const float fewHits = ApplyScoreBase(1.0f - HitScore, 0.5f);

	// 攻撃成功の多さ（未配線のため常に ScoreByHitCount(0)=1 寄り）
	const float manySuccess = ApplyScoreBase(ScoreByHitCount(attackSuccessCount, hitCoeff), 0.7f);

	// !!!New!!!
	const float probeContinuePowRate = enemyData.probeContinuePowRate;
	const float holdContinuePowRate = enemyData.holdContinuePowRate;
	const float escapeContinuePowRate = enemyData.escapeContinuePowRate;

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
		scoreProbe *= std::pow(stanceContinue, probeContinuePowRate);
		break;
	case AIStance::HoldAdvantage:
		scoreHold *= std::pow(stanceContinue, holdContinuePowRate);
		break;
	case AIStance::EscapeDisadvantage:
		scoreEscape *= std::pow(stanceContinue, escapeContinuePowRate);
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

	DebugConsole::SetDrawPos(15, 16);
	std::cout << "被弾カウント" << hitTakenCount << std::endl;
	return newStance;
}

float AIManager::ApplyScoreBase(float score, float base)
{
	// 基準を基に正規化
	float normalizedScore = (score * (1.0f - base)) + base;
	// 基準値を掛ける
	return normalizedScore;
}

// 距離用カーブ、mid に近いほど値が大きくなる
float AIManager::ScoreByDistance(float dist, float mid, float coefficient)
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
float AIManager::ScoreByHitCount(int hitCount, float coefficient)
{
	if (coefficient <= 0.0f)
	{
		coefficient = 0.0001f;
	}
	float score = 1.0f - std::exp(-static_cast<float>(hitCount) / coefficient);
	return score;
}


// 方針継続カーブ、回数に応じて coefficient で割っていく（coefficient>1 前提）
float AIManager::ScoreByStance(int stanceCount, float coefficient)
{
	if (coefficient <= 1.0f)
	{
		coefficient = 1.0001f;
	}
	return min(1.0f / std::pow(coefficient, static_cast<float>(stanceCount)), 1.0f);
}