#pragma once
#include <string>
#include <optional>
#include <unordered_map>
#include <vector>

#include "ECSData.h"
#include "ISystemResponse.h"


class SystemResponse : public ISystemResponse
{
public:
	SystemResponse();
	~SystemResponse();

	virtual void AddStopTime(float a_addTime, float a_maxStopTime = FLT_MAX, float a_stopRate = 0.1f) override;
	
	// Deltatime調整
	void StepStopTime(float a_deltatime);
	void ResetStopTime();
	bool IsStop();

	// ワールド切り替え
	virtual void CreateWorldRequest(std::string a_key) override;
	std::string GetWorldRequest();
	virtual void ResetRequest();
	bool IsWorldRequest();
	float GetStopRate();

	// Prefabの生成リクエスト
	virtual void AddEntitySpawnRequest(SpawnRequest a_spawnRequest) override;
	const std::vector<SpawnRequest>& GetEntitySpawnRequests() const;

private:
	float stopTime;
	float stopRate;
	bool isWorldRequest;
	std::string worldRequest;
	std::vector<SpawnRequest> entitySpawnRequests;

};

