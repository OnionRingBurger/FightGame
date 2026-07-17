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
		std::function<void(int)> tutorialRequest,
		Input& a_input);
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

	std::unique_ptr<SystemResponse> systemResponse;
	WorldState m_state;
	bool isMouseLock;
	SystemContext context;
	bool isGameEnd;
	bool allLoadModel = false;


protected:

	virtual void InitChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_systemResponse);

	virtual Chunk CreateNewChunk();

	virtual void UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_systemResponse);

	virtual void UpdateIMGUI(Chunk& a_chunk, SystemContext& a_context);

	virtual void HandleSystemResponse(SystemResponse& a_response);

};

#endif // !



