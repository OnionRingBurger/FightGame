#pragma once
#include <string>
class ISystemResponse
{
public:
	ISystemResponse();
	~ISystemResponse() = default;

	virtual void AddStopTime(float a_addTime, float a_maxStopTime = FLT_MAX, float a_stopRate = 0.1f) = 0;
	virtual void CreateWorldRequest(std::string a_key) = 0;

};

