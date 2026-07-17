#pragma once
#include "Chunk.h"
#include "SystemContext.h"

void VelocitySystem(Chunk& a_chunk, const SystemContext& a_context);

void AngularVelocitySystem(Chunk& a_chunk, const SystemContext& a_context);