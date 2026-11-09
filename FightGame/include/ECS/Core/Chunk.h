#ifndef ___CHUNK_H___
#define ___CHUNK_H___

#include <memory>
#include "EntityManager.h"
#include "StorageManager.h"
#include "ComponentView.h"
#include <vector>

using DeleteView = std::vector<Entity>::const_iterator;

class Chunk
{
public:

	Chunk();
	~Chunk() = default;
	// エンティティを作成
	template<typename ...Components>
	Entity CreateNewEntity(Components... a_components);

	void ImGuiInPut();

	void ImGuiOutPut();

	template<typename T>
	ComponentHandle<T> GetComponent(Entity);

	template<typename T>
	void AddComponent(Entity, T);

	void DeleteChunkEntity(Entity);

	std::vector<Entity> GetAllEntity();
	HasView GetHasView(Entity);

	template<typename UseComponentsTypes>
	ComponentView GetView();
	template<typename UseComponentsTypes, typename ExclusionComponentsTypes>
	ComponentView GetView();

	DeleteView GetDeleteView();

	void DeleteChunkComponent(Entity, TypeID a_typeId);

	void CleanupFrame();

private:
	// 各マネージャー
	EntityManager entitesManager;
	StorageManager storageManager;

	// 削除予定のComponentやEntityを保持
	std::vector<std::pair<Entity, TypeID>> deleteComponents;
	std::vector<Entity> deleteEntities;


};

template<typename ...Components>
inline Entity Chunk::CreateNewEntity(Components... a_components)
{
	Entity entity = entitesManager.CreateEntity();
	(AddComponent(entity, a_components),...);
	return entity;
}

template<typename T>
inline ComponentHandle<T> Chunk::GetComponent(Entity a_entity)
{
	if (!entitesManager.IsEntityIsAlive(a_entity)) return ComponentHandle<T>();


	// Componentのポインタを返す
	return storageManager.GetComponent<T>(a_entity);
}

template<typename T>
inline void Chunk::AddComponent(Entity a_entity, T a_component)
{
	if (!entitesManager.IsEntityIsAlive(a_entity)) return;

	storageManager.AddComponent<T>(a_entity, a_component);
}

template<typename UseComponentsTypes>
inline ComponentView Chunk::GetView()
{
	return storageManager.GetView<UseComponentsTypes>();
}

template<typename UseComponentsTypes, typename ExclusionComponentsTypes>
inline ComponentView Chunk::GetView()
{
	return storageManager.GetView<UseComponentsTypes, ExclusionComponentsTypes>();
}




#endif


