#pragma once
#include "World.h"

class TestWorld : public World
{
public:
	TestWorld(
		IModelCacheAcquisition& a_modelCache,
		IUICacheAcquisition& a_uiCache,
		std::function<void(int)> a_tutorialRequest,
		std::function<void(std::string)> a_worldRequest,
		Input& a_input
	);

protected:
	virtual Chunk CreateNewChunk() override;

	virtual void UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response) override;

	virtual void HandleSystemResponse(SystemResponse& a_response) override;

	std::function<void(std::string)> worldRequest;
};

