#pragma once
#include <functional>
#include <map>
#include <any>
#include "MathAssist.h"

using EntityID = size_t;
using Generation = size_t;
using TypeID = uint32_t;

struct Entity
{
	EntityID id;
	Generation generation;

	bool operator==(const Entity& other) const
	{
		return this->generation == other.generation && this->id == other.id;
	}

	bool operator!=(const Entity& other) const
	{
		return this->generation != other.generation || this->id != other.id;
	}
};


template<>
struct std::hash<Entity>
{
	size_t operator()(const Entity& e) const noexcept
	{
		return (static_cast<size_t>(e.id) << 32) ^ static_cast<size_t>(e.generation);
	}
};


class RigidTransform
{
public:

	RigidTransform(float3 a_position, float3 a_rotation, float3 a_scale);
	~RigidTransform() = default;

	float3 MultiplyPoint(float3);

	float3 MultiplyVector(float3);

private:

	float3 position;
	float rotation[3][3];
	float3 scale;
};

// Componentを保存するための構造体
struct ComponentData
{
	TypeID id;
	std::any componrnt;

	ComponentData(TypeID a_id, std::any a_component)
	: id( a_id)
	, componrnt(a_component)
	{
	}
};

// Prefab本体
struct EntityPrefab
{
	// データを保持し、Entity同士の対応を持たせるための仮想Entityデータ
	Entity entity;
	// 全ての保持コンポーネント
	std::vector<ComponentData> haveComponents;

	EntityPrefab(Entity a_entity, const std::vector<ComponentData>& a_haveComponents)
	: entity(a_entity)
	, haveComponents((a_haveComponents))
	{
	}

};

struct EntitySpawnRequest
{
	std::vector<EntityPrefab> prefabs;
};



using EntityMap = std::map<int, Entity>;