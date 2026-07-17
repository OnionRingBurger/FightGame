#pragma once
#include <functional>
#include <map>
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

using EntityMap = std::map<int, Entity>;