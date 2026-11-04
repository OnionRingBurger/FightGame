#include "AI/StanceNode.h"

#include <algorithm>
#include <cmath>

#include "MathAssist.h"
#include "DebugConsole.h"
#include "AI/RoleCurve.h"

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

Node::Status StanceNode::Tick(AIContext& context, const AIBlackboard& blackBoard, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result, float deltaTime)
{

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
	return childNodes.at(stanceIndex)->Tick(context, blackBoard, mind, systemInfo, result, deltaTime);
}



