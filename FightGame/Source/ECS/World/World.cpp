#include "World.h"
#include "Components.h"
#include "System.h"
#include "Sound.h"
#include "GameData.h"
#include <unordered_map>

#include "Defines.h"
#include <fstream>


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
	ComponentsSerialize& a_serialize,
	PrefabManager& a_prefabManager)
	: prefabManager(a_prefabManager)
	, systemResponse()
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
	InitChunk(chunk, context, *systemResponse, aiManager, serialize);

	// ファイルを読み込む
	std::ifstream stream(kDataPath);
	nlohmann::json json;
	stream >> json;

	std::string name = "Stage1Phase1";
	prefabManager.CreatePrefabScene(name);
	std::vector<EntityPrefab> vec = LoadPrefab(json, serialize, name);
	for (auto it : vec)
	{
		prefabManager.AddPrefab(name, it);
	}
}

void World::InitResponse(std::unique_ptr<SystemResponse>& response)
{
	systemResponse = std::make_unique<SystemResponse>();
}

void World::InitChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_systemResponse, AIManager& a_aiManager, ComponentsSerialize& a_serialize)
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

	FrameEnd();

}

void World::FrameEnd()
{
	chunk.CleanupFrame();

	if (systemResponse == nullptr) return;
	HandleSystemResponse(*systemResponse);

	RequestSpawn();

	systemResponse->ResetRequest();
}

void World::RequestSpawn()
{
	for(auto incomingRequest : systemResponse->GetEntitySpawnRequests())
	{	
		EntitySpawnRequest requestData;
		prefabManager.TryGetPrefabScene(incomingRequest.key, requestData);
	
		std::vector<Entity> entities = spawnPort.AddEntities(chunk, requestData);
		SpawnPostProcessing(entities, chunk, aiManager, incomingRequest);
	}
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

void World::SpawnPostProcessing(const std::vector<Entity>& entities, Chunk& a_chunk, AIManager& a_aiManager, const SpawnRequest& a_request)
{
}


