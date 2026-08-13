#pragma once

#include "Chunk.h"
#include "SystemContext.h"

void InputMoveSystem(Chunk& a_chunk, const SystemContext& a_context);

void InputRotatoSystem(Chunk& a_chunk, const SystemContext& a_context);

void LookMoveSystem(Chunk& a_chunk, const SystemContext& a_context);

void FlipSystem(Chunk& a_chunk, const SystemContext& a_context);

void FollowTransformSystem(Chunk& a_chunk, const SystemContext& a_context);

void TrackingWarpSystem(Chunk& a_chunk, const SystemContext& a_context);

void LeapSystem(Chunk& a_chunk, const SystemContext& a_context);

void ShakingSystem(Chunk& a_chunk, const SystemContext& a_context);

void LookSystem(Chunk& a_chunk, const SystemContext& a_context);

void PoseSystem(Chunk& a_chunk, const SystemContext& a_context);

void LatePoseSystem(Chunk& a_chunk, const SystemContext& a_context);

void TransformSystem(Chunk& a_chunk, const SystemContext& a_context);

// !!!New!!!
void PhysicsPoseSystem(Chunk& a_chunk, const SystemContext& a_context);

// !!!New!!!
void LatePhysicsPoseSystem(Chunk& a_chunk, const SystemContext& a_context);

void ShakeSystem(Chunk& a_chunk, const SystemContext& a_context);

void MoveForwardSystem(Chunk& a_chunk, const SystemContext& a_context);

