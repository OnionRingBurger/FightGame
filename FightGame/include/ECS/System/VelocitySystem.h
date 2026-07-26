#pragma once
#include "Chunk.h"
#include "SystemContext.h"

void MoveForwardSystem(Chunk& a_chunk, const SystemContext& a_context);

void ForceSystem(Chunk& a_chunk, const SystemContext& a_context);

void WorldPowerSystem(Chunk& a_chunk, const SystemContext& a_context);