#include "DebugSystemResponse.h"

DebugSystemResponse::DebugSystemResponse(std::string a_key)
	:isSave(false)
	,isLoad(false)
	,key(a_key)
{
}

DebugSystemResponse::~DebugSystemResponse()
{
}

void DebugSystemResponse::ResetRequest() 
{
	SystemResponse::ResetRequest();
	isSave = false;
	isLoad = false;
	key.clear();
}

void DebugSystemResponse::SaveRequest()
{
	isSave = true;
}

bool DebugSystemResponse::IsSave()
{
	return isSave;
}

void DebugSystemResponse::LoadRequest(std::string a_key)
{
	isLoad = true;
	key = std::move(a_key);
}

bool DebugSystemResponse::IsLoad()
{
	return isLoad;
}

const std::string& DebugSystemResponse::GetKey() const
{
	return key;
}
