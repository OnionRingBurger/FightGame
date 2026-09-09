#pragma once
#include "Chunk.h"
#include "SystemContext.h"
#include "ComponentsSerialize.h"
#include "AIManager.h"

void ResetSystem(Chunk& a_chunk, const SystemContext& a_context);

void LifeTimeSystem(Chunk& a_chunk, const SystemContext& a_context);

void SpawnJsonSystem(Chunk& a_chunk, const SystemContext& a_context, ComponentsSerialize& a_serialize, AIManager& a_aimanager);