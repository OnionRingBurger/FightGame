#pragma once
#include "Chunk.h"
#include "SystemContext.h"
#include "ComponentsSerialize.h"
#include "AIManager.h"
#include "ISystemResponse.h"

void ResetSystem(Chunk& a_chunk, const SystemContext& a_context);

void LifeTimeSystem(Chunk& a_chunk, const SystemContext& a_context);

void DeleteOnInputSystem(Chunk& a_chunk, const SystemContext& a_context);

void SpawnJsonSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_response);