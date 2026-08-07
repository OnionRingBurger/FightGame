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

	// !!!New!!!
	void WriteSense(const AISenseFrame& senseFrame);

	// !!!New!!!
	void WriteSystem(const AISystemInfo& a_systemInfo);

	// !!!New!!!
	void NotifyAttackResolved(Entity attacker, int attackIndex, bool connected);

	void NotifyHitResolved(Entity hitEnemy);

	// !!!New!!!
	void TickAI();

private:
	// 種類ごとのビヘイビアツリー
	BTMap behavioreTreeMap;

	// 各エンティティのと対応するAIの情報
	RuntimeMap runtimeMap;
	// 共有ブラックボード
	AIBlackboard blackBoard;
	// システム側の情報
	AISystemInfo systemInfo;
};
