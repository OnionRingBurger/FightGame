#include "DebugSystemResponse.h"

DebugSystemResponse::DebugSystemResponse()
	:isSave(false)
	,isLoad(false)
	,key()
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

void DebugSystemResponse::SaveRequest(std::string a_key)
{
	isSave = true;
	key = std::move(a_key);
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
