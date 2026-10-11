#pragma once
#include <string>
#include "MathAssist.h"

struct SpawnRequest
{
	std::string key;
	float3 offset;

	SpawnRequest(std::string a_key, float3 a_offset)
		: key(a_key)
		, offset(a_offset)
	{
	}
};

class ISystemResponse
{
public:
	ISystemResponse();
	~ISystemResponse() = default;

	virtual void AddStopTime(float a_addTime, float a_maxStopTime = FLT_MAX, float a_stopRate = 0.1f) = 0;
	virtual void CreateWorldRequest(std::string a_key) = 0;
	virtual void AddEntitySpawnRequest(SpawnRequest a_spawnKey) = 0;

};

