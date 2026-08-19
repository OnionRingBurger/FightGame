#pragma once
#include "World.h"
class GameWorld : public World
{
public:
	GameWorld(IModelCacheAcquisition& a_modelCache,
		IUICacheAcquisition& a_uiCache,
		std::function<void(int)> a_tutorialRequest,
		std::function<void(std::string)> a_worldRequest,
		Input& a_input,
		ComponentsSerialize& a_serialize);

protected:
	virtual void InitChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response) override;

	virtual Chunk CreateNewChunk(AIManager& a_aiManager) override;

	virtual void UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response, AIManager& a_aiManager, ComponentsSerialize& a_serialize) override;

	virtual void HandleSystemResponse(SystemResponse& a_response) override;

	std::function<void(std::string)> worldRequest;
};

