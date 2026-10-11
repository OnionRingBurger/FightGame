#ifndef ___WORLD_H___
#define ___WORLD_H___


#include <memory>
#include <vector>
#include <array>
#include <functional>

#include "Chunk.h"
#include "SystemContext.h"
#include "ECSData.h"
#include "SystemResponse.h"
#include "Loading.h"
#include "ThreadSafeQueue.h"
#include "IScene.h"
#include "IModelCacheAcquisition.h"
#include "IUICacheAcquisition.h"
#include "IEffectCacheAcquisition.h"
#include "AI/AIManager.h"
#include "ComponentsSerialize.h"
#include "EntitySpawnPort.h"
#include "PrefabManager.h"

enum WorldState
{
	USECHUNK,
	WAIT,
	IMGUI
};


class World
{
public:

	
	World(
		IModelCacheAcquisition& modelCache,
		IUICacheAcquisition& uiCache,
		IEffectCacheAcquisition& effectCache,
		std::function<void(int)> tutorialRequest,
		Input& a_input,
		ComponentsSerialize& a_serialize,
		PrefabManager& a_prefabManager);
	~World();

	void InitWorld();

	void ContextUpdate(float a_delatTime);

	void UpdateWorld();

	void DrawWorld();

	bool IsEnd();

	void ChangeState(WorldState state);

private:

	void InitContext();
	void FrameEnd();

	void RequestSpawn();

	// 各機能
	Chunk chunk;
	AIManager aiManager;
	EntitySpawnPort spawnPort;
	PrefabManager& prefabManager;
	std::unique_ptr<SystemResponse> systemResponse;
	// WorldのState等
	WorldState m_state;
	bool isMouseLock;
	SystemContext context;
	bool isGameEnd;
	bool allLoadModel = false;
	float aiTickAccum = 0.0f;


	ComponentsSerialize& serialize;

	// 3F起きに更新
	static constexpr float kAITickInterval = 3.0f / 60.0f;

protected:

	virtual void InitResponse(std::unique_ptr<SystemResponse>& response);

	virtual void InitChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_systemResponse, AIManager& a_aiManager, ComponentsSerialize& a_serialize);

	virtual void InitAI(AIManager& a_aiManager);

	virtual Chunk CreateNewChunk(AIManager& a_aiManager);

	virtual void UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_systemResponse, AIManager& a_aiManager, ComponentsSerialize& a_serialize);

	virtual void UpdateIMGUI(Chunk& a_chunk, SystemContext& a_context);

	virtual void HandleSystemResponse(SystemResponse& a_response);

	virtual void SpawnPostProcessing(const std::vector<Entity>& entities, Chunk& a_chunk, AIManager& a_aiManager, const SpawnRequest& a_request);
};

#endif // !



