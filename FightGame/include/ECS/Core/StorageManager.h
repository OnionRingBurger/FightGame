#pragma once

#include "StorageManager.h"
#include "IComponentStorage.h"
#include "ComponentStorage.h"
#include <unordered_map>
#include <memory>
#include <typeinfo>
#include "ComponentView.h"
#include "HasView.h"
#include "DebugConsole.h"

template<typename... T>
using ComponentTypes = std::tuple<T...>;

class StorageManager
{
public:
	StorageManager();
	~StorageManager();

	template<typename T>
	ComponentHandle<T> GetComponent(Entity& entity);

	HasView GetHasView(Entity a_entity);
	
	template<typename T>
	void AddComponent(Entity, T);
	/**
	 * @fn
	 * コンポーネントを削除する
	 * @brief 保持しているストレージ内のコンポーネントを削除する
	 * @param a_entity コンポーネントを削除するエンティティ
	 * @param a_type 削除するコンポーネントのタイプ
	 */
	void DeleteComponent(Entity a_entity, const TypeID a_type);

	
	template<typename UseComponentTypes>
	ComponentView GetView();

	template<typename UseComponentTypes, typename ExclusionComponentsTypes>
	ComponentView GetView();

private:
	template<typename T>
	ComponentStorage<T>& GetStorage();
	

	std::unordered_map<TypeID, std::shared_ptr<IComponentStorage>> storages;

	template<typename Tuple, std::size_t... Is>
	void AddStoragesFromTupleImpl(std::index_sequence<Is...>, std::vector<std::vector<Entity>>& useEntities);
};



template<typename T>
inline ComponentHandle<T> StorageManager::GetComponent(Entity& a_entity)
{
	// Storageを取得
	ComponentStorage<T>& storage = GetStorage<T>();

	// Componentのハンドルを返す
	return storage.Get(a_entity);
}

template<typename T>
inline void StorageManager::AddComponent(Entity entity, T components)
{
	// ハッシュ値を作成しStorageが存在するか確認
	TypeID id = T::kTypeId;
	auto it = storages.find(id);

	// 存在しなかった場合新たにStorageを作成
	if (it == storages.end())
	{
		storages[id] = std::make_shared<ComponentStorage<T>>();
	}

	// storagesをキャストし、値を返す
	ComponentStorage<T>* storage = static_cast<ComponentStorage<T>*>(storages.at(id).get());
		
	storage->Add(entity, components);
}




template<typename UseComponentTypes>
inline ComponentView StorageManager::GetView()
{
	// view作成用にstorageの保持しているEntityを保持
	std::vector<std::vector<Entity>> useEntities;
	
	// tupleを展開して型を読み込みその型のstorageの持っているEntityIDを取得する
	AddStoragesFromTupleImpl<UseComponentTypes>(std::make_index_sequence<std::tuple_size<UseComponentTypes>::value>{}, useEntities);
	
	// viewを作成し返す
	return ComponentView(useEntities);
}

template<typename UseComponentTypes, typename ExclusionComponentsTypes>
inline ComponentView StorageManager::GetView()
{
	// view作成用にstorageの保持しているEntityを保持
	std::vector<std::vector<Entity>> useEntities;
	std::vector<std::vector<Entity>> exclusionEntities;

	// tupleを展開して型を読み込みその型のstorageの持っているEntityIDを取得する
	AddStoragesFromTupleImpl<UseComponentTypes>(std::make_index_sequence<std::tuple_size<UseComponentTypes>::value>{}, useEntities);
	AddStoragesFromTupleImpl<ExclusionComponentsTypes>(std::make_index_sequence<std::tuple_size<ExclusionComponentsTypes>::value>{}, exclusionEntities);

	// viewを作成し返す
	return ComponentView(useEntities, exclusionEntities);
}

template<typename T>
inline ComponentStorage<T>& StorageManager::GetStorage()
{

	// ハッシュ値を作成しStorageが存在するか確認
	uint32_t id = T::kTypeId;
	auto it = storages.find(id);

	// 存在しなかった場合新たにStorageを作成
	if (it == storages.end())
	{
		storages[id] = std::make_shared<ComponentStorage<T>>();
	}

	// Componentの参照を返す
	return *static_cast<ComponentStorage<T>*>(storages.at(id).get());
}

template<typename Tuple, std::size_t ...Is>
inline void StorageManager::AddStoragesFromTupleImpl(std::index_sequence<Is...>, std::vector<std::vector<Entity>>& useEntities)
{
	// tupleを展開し型を取得、その型のstorageを呼び出し持っているEntityを取得
	((useEntities.push_back(GetStorage<std::tuple_element_t<Is, Tuple>>().GetHaveEntities()), ...));
}

