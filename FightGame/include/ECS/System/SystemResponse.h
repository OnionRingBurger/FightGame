#pragma once
#include <string>
#include <optional>
#include <unordered_map>

#include "ECSData.h"
#include "ISystemResponse.h"


class SystemResponse : public ISystemResponse
{
public:
	SystemResponse();
	~SystemResponse();

	virtual void AddStopTime(float a_addTime, float a_maxStopTime = FLT_MAX, float a_stopRate = 0.1f) override;
	
	void StepStopTime(float a_deltatime);
	void ResetStopTime();
	bool IsStop();

	virtual void CreateWorldRequest(std::string a_key) override;
	std::string GetWorldRequest();
	virtual void ResetRequest();
	bool IsWorldRequest();
	float GetStopRate();

private:
	float stopTime;
	float stopRate;
	bool isWorldRequest;
	std::string worldRequest;

};

