#include "SystemResponse.h"


SystemResponse::SystemResponse()
	: stopTime(0.0f)
	, stopRate(0.0f)
	, isWorldRequest()
	, worldRequest()
{
	ResetRequest();
}

SystemResponse::~SystemResponse()
{
}

ISystemResponse::ISystemResponse()
{
}

void SystemResponse::AddStopTime(float a_addTime, float a_maxStopTime, float a_stopRate)
{
    stopTime += std::min(a_addTime, a_maxStopTime);
	stopRate = a_stopRate;
}


void SystemResponse::StepStopTime(float a_deltatime)
{
    stopTime = std::max(stopTime - a_deltatime, 0.0f);
}


void SystemResponse::ResetStopTime()
{
    stopTime = 0.0f;
}

bool SystemResponse::IsStop()
{
    return stopTime > 0.0f;
}

void SystemResponse::CreateWorldRequest(std::string a_key)
{
	worldRequest = a_key;
	isWorldRequest = true;
}

std::string SystemResponse::GetWorldRequest()
{
	return worldRequest;
}


void SystemResponse::ResetRequest()
{
	worldRequest = "";
	isWorldRequest = false;
}

bool SystemResponse::IsWorldRequest()
{
	return isWorldRequest;
}

float SystemResponse::GetStopRate()
{
	return stopRate;
}



