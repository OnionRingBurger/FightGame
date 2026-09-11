#pragma once
#include <string>
#include <optional>
#include <unordered_map>

#include "ECSData.h"
#include "SystemResponse.h"

class DebugSystemResponse : public SystemResponse
{
public:
	DebugSystemResponse(std::string key);
	~DebugSystemResponse();

	virtual void ResetRequest() override;

	void SaveRequest();
	bool IsSave();

	// !!!New!!!
	void LoadRequest(std::string a_key);
	bool IsLoad();
	const std::string& GetKey() const;

private:
	bool isSave;
	// !!!New!!!
	bool isLoad;
	std::string key;
};
