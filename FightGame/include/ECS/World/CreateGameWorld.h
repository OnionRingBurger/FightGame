#pragma once
#include "World.h"
class CreateGameWorld : public World
{
public:
	CreateGameWorld(IModelCacheAcquisition& a_modelCache,
		IUICacheAcquisition& a_uiCache,
		std::function<void(int)> a_tutorialRequest,
		Input& a_input, 
		ComponentsSerialize& a_serialize
	);
	~CreateGameWorld() = default;

private:
	Chunk CreateNewChunk(AIManager& a_aiManager) override;

	void InitResponse(std::unique_ptr<SystemResponse>& response) override;

	void UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_systemResponse, AIManager& a_aiManager, ComponentsSerialize& a_serialize) override;

	void UpdateIMGUI(Chunk& a_chunk, SystemContext& a_context) override;

	void HandleSystemResponse(SystemResponse& a_response) override;

	void CreateDebugComponent(Chunk& a_chunk);

	std::string name;

	bool isSave;
};

