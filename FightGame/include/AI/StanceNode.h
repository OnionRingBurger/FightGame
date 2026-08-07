#pragma once

#include <memory>

#include "CompositeNode.h"

// Stance を更新し、Stance に応じて子 Node を選んで実行する Composite
class StanceNode : public CompositeNode
{
public:
	// コンストラクタでノードを登録する
	StanceNode(
		std::unique_ptr<Node> escapeDisadvantage,
		std::unique_ptr<Node> holdAdvantage,
		std::unique_ptr<Node> neutralProbe);
	~StanceNode() override;

	Status Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result) override;

private:

	static float GetNewUpdatePoint(const AIBlackboard& blackBoard, const AIMind& mind, const AISystemInfo& systemInfo);
	static AIStance GetNewStance(const AIContext& context, const AIBlackboard& blackBord, const AIMind& mind);
	static float ScoreByDistance(float dist, float mid, float coefficient);
	static float ScoreByHitCount(int hitCount, float coefficient);
	static float ScoreByStance(int stanceCount, float coefficient);
	static float ApplyScoreBase(float score, float base);
};
