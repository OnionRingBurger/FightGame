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
		ComponentsSerialize& a_serialize);
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

	Chunk chunk;
	// !!!New!!!
	AIManager aiManager;
	std::unique_ptr<SystemResponse> systemResponse;
	WorldState m_state;
	bool isMouseLock;
	SystemContext context;
	bool isGameEnd;
	bool allLoadModel = false;
	// !!!New!!!
	float aiTickAccum = 0.0f;

	ComponentsSerialize& serialize;

	// 3FãNÇ´Ç…çXêV
	static constexpr float kAITickInterval = 3.0f / 60.0f;

protected:

	virtual void InitResponse(std::unique_ptr<SystemResponse>& response);

	virtual void InitChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_systemResponse);

	// !!!New!!!
	virtual void InitAI(AIManager& a_aiManager);

	virtual Chunk CreateNewChunk(AIManager& a_aiManager);

	virtual void UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_systemResponse, AIManager& a_aiManager, ComponentsSerialize& a_serialize);

	virtual void UpdateIMGUI(Chunk& a_chunk, SystemContext& a_context);

	virtual void HandleSystemResponse(SystemResponse& a_response);

};

#endif // !



