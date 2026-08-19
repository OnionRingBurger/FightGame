#pragma once
#include <string>
#include <optional>
#include <unordered_map>

#include "ECSData.h"
#include "SystemResponse.h"

class DebugSystemResponse : public SystemResponse
{
public:
	DebugSystemResponse();
	~DebugSystemResponse();

	virtual void ResetRequest() override;

	void SaveRequest();
	bool IsSave();

private:
	bool isSave;
	
};