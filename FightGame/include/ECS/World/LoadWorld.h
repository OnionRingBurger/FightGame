#pragma once
#include "World.h"
class LoadWorld : public World
{
public:
	LoadWorld(IModelCacheAcquisition& a_modelCache, IUICacheAcquisition& a_uiCache, Input& a_input, ComponentsSerialize& a_serialize);
	~LoadWorld() = default;


protected:
	virtual Chunk CreateNewChunk(AIManager& a_aiManager) override;

	virtual void UpdateChunk(Chunk& a_chunk, SystemContext& a_context, SystemResponse& a_response, AIManager& a_aiManager, ComponentsSerialize& a_serialize) override;
};

