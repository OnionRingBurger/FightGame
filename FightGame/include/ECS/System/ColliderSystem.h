#pragma once
#include "Chunk.h"
#include "SystemContext.h"
#include "ISystemResponse.h"

void ColliderSystem(Chunk& a_chunk, const SystemContext& a_context);

void ColliderCheckSystem(Chunk& a_chunk, const SystemContext& a_context);

void ColliderBackSystem(Chunk& a_chunk, const SystemContext& a_context);

void RaySystem(Chunk& a_chunk, const SystemContext& a_context);