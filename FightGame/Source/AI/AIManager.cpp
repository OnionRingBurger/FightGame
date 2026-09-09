#include "AI/AIManager.h"

// !!!New!!!
AIManager::AIManager()
{
}

AIManager::~AIManager()
{
}

bool AIManager::RegisterTree(const std::string& aiType, std::unique_ptr<BehaviorTree> tree)
{
	// !!!New!!!
	if (!tree || aiType.empty())
	{
		return false;
	}
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

void AIManager::WriteSense(const AISenseFrame& senseFrame)
{
	// プレイヤーを
	blackBoard.playerPos = senseFrame.playerPos;
	blackBoard.isPlayerDead = senseFrame.isPlayerDead;
	// 敵情報はリセットして入れ直す
	blackBoard.enemyDatas.clear();
	for (const auto& pair : senseFrame.enemyDatas)
	{
		// 存在するか確認
		if (runtimeMap.find(pair.first) == runtimeMap.end())
		{
			continue;
		}
		blackBoard.enemyDatas[pair.first] = pair.second;
	}
}

void AIManager::WriteSystem(const AISystemInfo& a_systemInfo)
{
	// System情報を書き込む
	systemInfo = a_systemInfo;
}


// TODO 攻撃終了処理を時間がある時に良い感じにまとめる
// 自身の攻撃終了結果を受け取る
void AIManager::NotifyAttackResolved(Entity attacker, int attackIndex, bool connected)
{

	// !!!New!!!
	const auto it = runtimeMap.find(attacker);
	if (it == runtimeMap.end())
	{
		return;
	}

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



void AIManager::TickAI()
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

		// 更新を行う
		treeIt->second->Tick(runtime.context, blackBoard, runtime.mind, systemInfo, runtime.result);
	}
}