#pragma once
#include "Chunk.h"
#include "SystemContext.h"
#include "ISystemResponse.h"
#include "AIManager.h"
#include "ComponentsSerialize.h"

void GoalSystem(Chunk& a_chunk, const SystemContext& a_context);

void ChunkChangeSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_chunkRequest);

void CheckAliveTargetEnemySystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_systemResponse, AIManager& a_aiManager, ComponentsSerialize& a_serialize);