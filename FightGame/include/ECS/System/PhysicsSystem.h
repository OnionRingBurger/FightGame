#pragma once
#include "Chunk.h"
#include "SystemContext.h"

////////////////////////////////////////////////////
// •¨—‹““®‚ğÀ‘•‚·‚éƒVƒXƒeƒ€
////////////////////////////////////////////////////

void VelocitySystem(Chunk& a_chunk, const SystemContext& a_context);

void AngularVelocitySystem(Chunk& a_chunk, const SystemContext& a_context);

void ForceSystem(Chunk& a_chunk, const SystemContext& a_context);

void WorldPowerSystem(Chunk& a_chunk, const SystemContext& a_context);