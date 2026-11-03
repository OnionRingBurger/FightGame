#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "AIData.h"
#include "BehaviorTree.h"

using BTMap = std::unordered_map<std::string, std::unique_ptr<BehaviorTree>>;
using RuntimeMap = std::unordered_map<Entity, AIRuntime>;

// !!!New!!!
class AIManager
{
public:
	AIManager();
	~AIManager();

	bool RegisterTree(const std::string& aiType, std::unique_ptr<BehaviorTree> tree);

	bool RegisterAI(Entity entity, const std::string& aiType);
	bool UnregisterAI(Entity entity);

	AIResult ReadResult(Entity entity) const;

	// AI機能へのシーン情報を書き込み
	void WriteSense(const AISenseFrame& senseFrame);

	// AI機能へのシステム情報書き込み
	void WriteSystem(const AISystemInfo& a_systemInfo);

	// 攻撃情報読み取り
	void NotifyAttackResolved(Entity attacker, int attackIndex, bool connected);
	// 攻撃命中情報読み取り
	void NotifyHitResolved(Entity hitEnemy, bool isGuard);

	// 思考に基づいた挙動を更新
	void TickBehaviorTree();

	// AIの思考を更新
	void TickAllAIMind();

private:
	// 種類ごとのビヘイビアツリー
	BTMap behavioreTreeMap;

	// 各エンティティのと対応するAIの情報
	RuntimeMap runtimeMap;
	// 共有ブラックボード
	AIBlackboard blackBoard;
	// システム側の情報
	AISystemInfo systemInfo;




	// スタンス点更新
	void UpdateStancePoint(AIMind& mind, const AISystemInfo& systemInfo);
	// スタンス更新
	void UpdateStance(AIContext& context, const AIBlackboard& blackBoard, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result);
	
	void TickAIMind(AIContext& context, const AIBlackboard& blackBoard, AIMind& mind, AISystemInfo& systemInfo, AIResult& result);

	bool IsStanceUpdate(const AIMind& mind);

	AIStance GetNewStance(const AIContext& context, const AIBlackboard& blackBoard, const AIMind& mind);

	float ApplyScoreBase(float score, float base);

	float ScoreByDistance(float dist, float mid, float coefficient);

	float ScoreByHitCount(int hitCount, float coefficient);

	float ScoreByStance(int stanceCount, float coefficient);
};
