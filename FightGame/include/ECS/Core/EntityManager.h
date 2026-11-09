#ifndef ___ENTITY_MANAGER_H___
#define ___ENTITY_MANAGER_H___

#include "ECSTypes.h"
#include <vector>
#include <optional>

struct EntityState
{
	Generation generation;
	bool isAlive;
};

class EntityManager
{
public:
	EntityManager();
	~EntityManager();

	// エンティティ生成
	Entity CreateEntity();

	// エンティティ削除
	void DeleteEntity(Entity);

	// 渡されたEntityが生存しているか確認
	bool IsEntityIsAlive(Entity a_entity);

	void DefaultImGuiOutPut();

	std::vector<Entity> GetAllEntity();


private:

	std::vector<EntityState> entitys;
	
	size_t nextEntity;
	
};



#endif
