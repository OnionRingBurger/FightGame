#pragma once

#include "Chunk.h"
#include "SystemContext.h"

void CameraViewSystem(Chunk& a_chunk, const SystemContext& a_context);

void CameraMoveSystem(Chunk& a_chunk, const SystemContext& a_context);

void ZoomMoveSystem(Chunk& a_chunk, const SystemContext& a_context);
