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
	explicit AIManager(float behaviorTickInterval);
	~AIManager();

	bool RegisterTree(const std::string& aiType, std::unique_ptr<BehaviorTree> tree);

	bool RegisterAI(Entity entity, const std::string& aiType);
	bool UnregisterAI(Entity entity);

	AIResult ReadResult(Entity entity) const;

	// AI機能へのシーン情報を書き込み
	void WriteSenseFrame(const AISenseFrame& senseFrame);
	void WriteSenseProfile(const AISenseProfile& senseProfile);
	bool IsRegisteredAI(Entity entity) const;
	bool IsSenseProfileReady(Entity entity) const;

	// AI機能へのシステム情報書き込み（当面は空の受け取り）
	void WriteSystem(const AISystemInfo& a_systemInfo);

	// 攻撃情報読み取り
	void NotifyAttackResolved(Entity attacker, int attackIndex, bool connected);
	// 攻撃命中情報読み取り
	void NotifyHitResolved(Entity hitEnemy, bool isGuard);

	// フレーム数を受け取り内部で低頻度で更新
	void Tick(float deltaTime);

private:
	BTMap behavioreTreeMap;

	RuntimeMap runtimeMap;
	AIBlackboard blackBoard;
	AISystemInfo systemInfo;

	float behaviorTickInterval;
	float behaviorTickAccum;

	void TickBehaviorTree(float behaviorDeltaTime);
	void TickAllAIMind(float deltaTime);

	void UpdateStancePoint(AIMind& mind, float deltaTime);
	void UpdateStance(AIContext& context, const AIBlackboard& blackBoard, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result);

	void TickAIMind(AIContext& context, const AIBlackboard& blackBoard, AIMind& mind, float deltaTime, AIResult& result);

	bool IsStanceUpdate(const AIMind& mind);

	AIStance GetNewStance(const AIContext& context, const AIBlackboard& blackBoard, const AIMind& mind);

	float ApplyScoreBase(float score, float base);

	float ScoreByDistance(float dist, float mid, float coefficient);

	float ScoreByHitCount(int hitCount, float coefficient);

	float ScoreByStance(int stanceCount, float coefficient);
};
