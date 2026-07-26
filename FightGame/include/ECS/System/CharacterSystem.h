#pragma once
#include "Components.h"
#include "Chunk.h"
#include "SystemContext.h"
#include "SystemAssist.h"


void HitPointSystem(Chunk& a_chunk, SystemContext& a_context);

// !!!New!!!
void GroundedSystem(Chunk& a_chunk, const SystemContext& a_context);

// !!!New!!!
void CharacterActionMaskSystem(Chunk& a_chunk, const SystemContext& a_context);

// !!!New!!!
void AttackActionSystem(Chunk& a_chunk, const SystemContext& a_context);

// !!!New!!!
void AttackWaitActionSystem(Chunk& a_chunk, const SystemContext& a_context);

// !!!New!!!
void JumpPhysicsSystem(Chunk& a_chunk, const SystemContext& a_context);
