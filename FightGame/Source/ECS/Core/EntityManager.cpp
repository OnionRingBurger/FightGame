#include "EntityManager.h"
#include "imGui/imgui.h"
#include <string>


EntityManager::EntityManager() : nextEntity(0)
{
}

EntityManager::~EntityManager()
{
}

Entity EntityManager::CreateEntity()
{
	// 戻り値
	Entity ret;
	// 生存していないかどうかのフラグ
	bool isFind = false;
	// イテレータで回した
	EntityID id = 0;

	// 生存していないentityを操作
	for (auto& it : entitys)
	{
		// 生存していないentityを操作
		if (it.isAlive)
		{
			id++;
			continue;
		}


		entitys[id].isAlive = true;
		ret = Entity{ id, it.generation };
		isFind = true;
		break;
	}

	// 見つからなかった場合新しく作製
	if (!isFind)
	{
		ret = Entity{ entitys.size(), 0};
		
		entitys.push_back(EntityState{ 0, true });

	}

	return ret;
}

std::vector<Entity> EntityManager::GetAllEntity()
{
	std::vector<Entity> entities;
	size_t i = 0;
	for (auto it : entitys)
	{
		if (!it.isAlive)
		{
			i++;
			continue;
		}

		 entities.push_back(Entity{ i, it.generation });
		 i++;
	}

	return entities;
}

void EntityManager::DeleteEntity(Entity a_entity)
{
	if (!IsEntityIsAlive(a_entity)) return;

	// 世代を更新し作成されていない状態にする。
	entitys.at(a_entity.id).generation++;
	entitys.at(a_entity.id).isAlive = false;

}

bool EntityManager::IsEntityIsAlive(Entity a_entity)
{
	// 与えられたidのentityを確保
	if (entitys.size() <= a_entity.id) return false;

	EntityState state = entitys.at(a_entity.id);

	// 世代番号が間違っていたら返す
	if (state.generation != a_entity.generation) return false;
	
	// 存在しているか確認
	return state.isAlive;

}


void EntityManager::DefaultImGuiOutPut()
{
	std::string haveEntities = "Have entities count = ";
	haveEntities += std::to_string(entitys.size());
	ImGui::Text(haveEntities.c_str());
}
