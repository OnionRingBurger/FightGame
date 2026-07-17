#pragma once
#include "World.h"
class CreateGameWorld : public World
{
public:
	CreateGameWorld(IModelCacheAcquisition& a_modelCache,
		IUICacheAcquisition& a_uiCache,
		std::function<void(int)> a_tutorialRequest,
		Input& a_input
	);
	~CreateGameWorld() = default;

private:
	Chunk CreateNewChunk() override;

	void UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_systemResponse) override;

	void UpdateIMGUI(Chunk& a_chunk, SystemContext& a_context) override;

	void HandleSystemResponse(SystemResponse& a_response) override;
};

