#pragma once
#include <string>
#include <optional>
#include <unordered_map>

#include "ECSData.h"
#include "SystemResponse.h"

class GameSystemResponse : public SystemResponse
{
public:
	GameSystemResponse();
	~GameSystemResponse();

	virtual void ResetRequest() override;

	// !!!New!!!
	void StageRequest(int a_index);
	bool IsStageRequest();
	int GetStageIndex();

private:
	// !!!New!!!
	bool isStageRequest;
	int stageIndex;
};
