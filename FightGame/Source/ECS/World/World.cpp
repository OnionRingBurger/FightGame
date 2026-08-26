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
	IEffectCacheAcquisition& effectCache,
	std::function<void(int)> a_tutorialRequest,
	Input& a_input, 
	ComponentsSerialize& a_serialize)
	: systemResponse()
	, m_state(USECHUNK)
	, isMouseLock(true)
	, context(a_modelCache, a_uiCache, effectCache, a_tutorialRequest, a_input) // TODO System側が勝手にリクエストを触れないようにする
	, isGameEnd(false)
	, serialize(a_serialize)
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

	InitResponse(systemResponse);
	if (!systemResponse)	
	{
		systemResponse = std::make_unique<SystemResponse>();
	}

	// Tree 登録を先に行い、CreateNewChunk 内の RegisterAI が成功するようにする
	InitAI(aiManager);

	chunk = CreateNewChunk(aiManager);

	ContextUpdate(0.01f);
	InitChunk(chunk, context, *systemResponse);
}

void World::InitResponse(std::unique_ptr<SystemResponse>& response)
{
	systemResponse = std::make_unique<SystemResponse>();
}

void World::InitChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_systemResponse)
{

}

void World::InitAI(AIManager& a_aiManager)
{
	// !!!New!!!
	(void)a_aiManager;
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

	// AIの更新頻度を管理
	context.aiTickThisFrame = false;
	context.aiDeltaTime = 0.0f;
	aiTickAccum += context.deltaTime;
	if (aiTickAccum >= kAITickInterval)
	{
		context.aiTickThisFrame = true;
		context.aiDeltaTime = aiTickAccum;
		aiTickAccum = 0.0f;
	}
}

void World::UpdateWorld()
{

	// System処理
	switch (m_state)
	{
	case USECHUNK:
	{
		UpdateChunk(chunk, context, *systemResponse, aiManager, serialize);
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

Chunk World::CreateNewChunk(AIManager& a_aiManager)
{
	(void)a_aiManager;
	return Chunk();
}

void World::UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_systemResponse, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
{
	(void)a_chunk;
	(void)a_context;
	(void)a_systemResponse;
	(void)a_aiManager;
}

void World::UpdateIMGUI(Chunk& a_chunk, SystemContext& a_context)
{
	ImGuiSystem(a_chunk, a_context);
}

void World::HandleSystemResponse(SystemResponse& a_response)
{

}


