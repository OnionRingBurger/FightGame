#pragma once
#include "IComponentStorage.h"

#include <unordered_map>
#include <vector>
#include "ECSTypes.h"

// Componentを制御し、変更を確認するためのハンドル
template<typename T>
class ComponentHandle
{

public:
	ComponentHandle(T* a_component, size_t* a_viersion)
		: m_component(a_component)
		, m_viersion(a_viersion)
		, m_dirty(false)
	{
	}

	ComponentHandle()
		: m_component(nullptr)
		, m_viersion(nullptr)
		, m_dirty(false)
	{
	}

	~ComponentHandle() = default;



	bool IsValid() const
	{
		return m_component && m_viersion;
	}

	// Handleが使われた時数値が変更されたと断定
	T* operator->()
	{
		if (!m_dirty)
		{
			m_dirty = true;
			if(m_viersion)
			*m_viersion++;
		}
		
		return m_component;
	};

	// 値を変更せず使用する場合こちらを利用する
	const T& Look() const
	{
		return *m_component;
	}

private:
	bool m_dirty;
	T* m_component;
	size_t* m_viersion;
};

template<typename T>
class ComponentStorage : public IComponentStorage
{
public:
	ComponentStorage();

	~ComponentStorage();


	virtual bool Has(const Entity&) override;
	virtual void Delete(Entity) override;
	void Add(Entity, T components);
	ComponentHandle<T> Get(Entity);

	virtual std::vector<Entity>& GetHaveEntities() override;

	virtual std::vector<Entity>& GetchangedEntities() override;
	

private:

	
	struct ComponentPack
	{
		T component;
		size_t version;

		ComponentPack(T a_component, size_t a_version)
			:component(a_component)
			, version(a_version)
		{}
	};

	std::unordered_map<Entity, size_t> indexMap;
	std::vector<Entity>idMap;
	std::vector<ComponentPack> components;

	std::vector<Entity> changedEntities;

};




template<typename T>
inline ComponentStorage<T>::ComponentStorage()
{
}

template<typename T>
inline ComponentStorage<T>::~ComponentStorage()
{
}

template<typename T>
inline bool ComponentStorage<T>::Has(const Entity& a_entity)
{
	return indexMap.count(a_entity);
}

template<typename T>
inline void ComponentStorage<T>::Delete(Entity a_entity)
{
	if (!Has(a_entity)) return;

	// 使用する変数を作成
	Entity id = a_entity;
	size_t index = indexMap.at(id);

	// コンポーネントの削除
	components.at(index) = components.back();
	components.pop_back();

	// idの削除
	idMap.at(index) = idMap.back();
	idMap.pop_back();

	// indexMapの更新
	indexMap.erase(id);
	if(index >= idMap.size()) return;
	indexMap.at(idMap.at(index)) = index;
	
}

template<typename T>
inline void ComponentStorage<T>::Add(Entity a_entityID, T a_component)
{
	if (Has(a_entityID)) return;

	indexMap[a_entityID] = components.size();

	idMap.push_back(a_entityID);

	components.push_back(ComponentPack(a_component, 0));
	
}

template<typename T>
inline ComponentHandle<T> ComponentStorage<T>::Get(Entity a_entity)
{
	if (!Has(a_entity)) return ComponentHandle<T>();

	ComponentPack& pack = components.at(indexMap.at(a_entity));

	return ComponentHandle<T>(&pack.component, &pack.version) ;
}

template<typename T>
inline std::vector<Entity>& ComponentStorage<T>::GetHaveEntities()
{
	return idMap;
}

template<typename T>
inline std::vector<Entity>& ComponentStorage<T>::GetchangedEntities()
{
	return changedEntities;
}
