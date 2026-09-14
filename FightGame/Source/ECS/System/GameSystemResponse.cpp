#include "GameSystemResponse.h"

GameSystemResponse::GameSystemResponse()
	: isStageRequest(false)
	, stageIndex(1)
{
}

GameSystemResponse::~GameSystemResponse()
{
}

void GameSystemResponse::ResetRequest()
{
	SystemResponse::ResetRequest();
	isStageRequest = false;
	stageIndex = 1;
}

void GameSystemResponse::StageRequest(int a_index)
{
	isStageRequest = true;
	stageIndex = a_index;
}

bool GameSystemResponse::IsStageRequest()
{
	return isStageRequest;
}

int GameSystemResponse::GetStageIndex()
{
	return stageIndex;
}
