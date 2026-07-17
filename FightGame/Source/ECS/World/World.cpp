#include "World.h"
#include "Components.h"
#include "System.h"
#include "Sound.h"
#include "GameData.h"


////////////////////////////////
//
// コンストラクタ、デストラクタ
//
////////////////////////////////

World::World(
	IModelCacheAcquisition& a_modelCache,
	IUICacheAcquisition& a_uiCache,
	std::function<void(int)> a_tutorialRequest,
	Input& a_input)
	: systemResponse()
	, m_state(USECHUNK)
	, isMouseLock(true)
	, context(a_modelCache, a_uiCache, a_tutorialRequest, a_input) // TODO System側が勝手にリクエストを触れないようにする
	, isGameEnd(false)
{
}

World::~World()
{
	ResetSound();
}

//////////////////////////////
//
// 一連の処理
//
//////////////////////////////

void World::InitWorld()
{
	// Contextの初期化
	InitContext();

	systemResponse = std::make_unique<SystemResponse>();

	chunk = CreateNewChunk();


	ContextUpdate(0.01f);
	InitChunk(chunk, context, *systemResponse);
}

void World::InitChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_systemResponse)
{

}

void World::InitContext()
{

}

void World::ContextUpdate(float a_delatTime)
{
	// DeltaTimeの更新
	float normalizeDeltatime = a_delatTime / 15.0f;
	context.effectStepTime = normalizeDeltatime;
	context.deltaTime = normalizeDeltatime;
	
	if (systemResponse->IsStop())
	{
		systemResponse->StepStopTime(normalizeDeltatime);
		context.deltaTime *= systemResponse->GetStopRate();
	}
}

void World::UpdateWorld()
{

	// System処理
	switch (m_state)
	{
	case USECHUNK:
	{
		UpdateChunk(chunk, context, *systemResponse);
	}
	break;

	case IMGUI:
	{
		UpdateIMGUI(chunk, context);
	}
	break;
	}
}

void World::DrawWorld()
{
	// System処理
	DrawSystem(chunk, context);

	chunk.CleanupFrame();
	FrameEnd();

}

void World::FrameEnd()
{
	if (systemResponse == nullptr) return;
	HandleSystemResponse(*systemResponse);

	systemResponse->ResetRequest();
}

//////////////////////////////
//
// 外部から受け付ける処理
//
//////////////////////////////

bool World::IsEnd()
{
	return isGameEnd;
}


void World::ChangeState(WorldState state)
{
	m_state = state;
}


//////////////////////////////
//
// override可能な処理
//
//////////////////////////////

Chunk World::CreateNewChunk()
{
	return Chunk();
}

void World::UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_systemResponse)
{

}

void World::UpdateIMGUI(Chunk& a_chunk, SystemContext& a_context)
{
	ImGuiSystem(a_chunk, a_context);
}

void World::HandleSystemResponse(SystemResponse& a_response)
{

}


