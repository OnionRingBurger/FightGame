#pragma once
#include "Chunk.h"
#include "SystemContext.h"
#include "ISystemResponse.h"

////////////////////////////////////////////////////
// プレイヤー制御を扱うシステム
////////////////////////////////////////////////////

void PlayerDeadSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_response);

void PlayerJumpSystem(Chunk& a_chunk, const SystemContext& a_context);

void PlayerHealSystem(Chunk& a_chunk, const SystemContext& a_context);