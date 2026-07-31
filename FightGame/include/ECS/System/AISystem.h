#pragma once

#include "Chunk.h"
#include "SystemContext.h"
#include "AI/AIManager.h"

// !!!New!!!
void AISenseSystem(Chunk& a_chunk, const SystemContext& a_context, AIManager& a_aiManager);

// !!!New!!!
void MoveInputResolveSystem(Chunk& a_chunk, const SystemContext& a_context, AIManager& a_aiManager);
