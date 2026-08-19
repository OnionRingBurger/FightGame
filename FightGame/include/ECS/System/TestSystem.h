#pragma once
#include "Chunk.h"
#include "SystemContext.h"
#include "ISystemResponse.h"
#include "AIManager.h"
#include "ComponentsSerialize.h"

void TestSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& response, AIManager& aiManager, ComponentsSerialize& serialize);
