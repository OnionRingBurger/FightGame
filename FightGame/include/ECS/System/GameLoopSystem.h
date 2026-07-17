#pragma once
#include "Chunk.h"
#include "SystemContext.h"
#include "ISystemResponse.h"

void GoalSystem(Chunk& a_chunk, const SystemContext& a_context);

void ChunkChangeSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_chunkRequest);