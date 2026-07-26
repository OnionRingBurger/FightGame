#pragma once
#include "Chunk.h"
#include "SystemContext.h"

////////////////////////////////////////////////////
// プレイヤー制御を扱うシステム
////////////////////////////////////////////////////

void PlayerWalkSystem(Chunk& a_chunk, const SystemContext& a_context);

void PlayerDeadSystem(Chunk& a_chunk, const SystemContext& a_context);

// !!!New!!!
void PlayerAttackSystem(Chunk& a_chunk, const SystemContext& a_context);

void PlayerJumpSystem(Chunk& a_chunk, const SystemContext& a_context);