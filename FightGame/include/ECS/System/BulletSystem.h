#pragma once
#include "Chunk.h"
#include "SystemContext.h"
#include "ISystemResponse.h"

void LaserSystem(Chunk& a_chunk, const SystemContext& a_context);

void BulletSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_chunkRequest);