#include "DebugSystemResponse.h"

DebugSystemResponse::DebugSystemResponse()
	:isSave(false)
{
}

DebugSystemResponse::~DebugSystemResponse()
{
}

void DebugSystemResponse::ResetRequest() 
{
	SystemResponse::ResetRequest();
	isSave = false;
}

void DebugSystemResponse::SaveRequest()
{
	isSave = true;
}

bool DebugSystemResponse::IsSave()
{
	return isSave;
}
