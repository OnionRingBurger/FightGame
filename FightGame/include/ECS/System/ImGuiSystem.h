#pragma once
#include "Chunk.h"
#include "SystemContext.h"
#include "SystemResponse.h"

void ImGuiSystem(Chunk& a_chunk, const SystemContext& a_context);

void SaveWorldSystem(Chunk& a_chunk, const SystemContext& a_context, SystemResponse& response);

void UndoWorldSystem(Chunk& a_chunk, const SystemContext& a_context);