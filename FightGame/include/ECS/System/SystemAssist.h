#pragma once
#include "Chunk.h"
#include "SystemContext.h"
#include "MathAssist.h"
#include "Components.h"


Entity GetPlayer(Chunk& a_chunk, float3 a_position);

Entity GetRail(Chunk& a_chunk);

float3 GetEntityWorldPos(Chunk& a_chunk, Entity a_entity);

void ResetFixedPosState(ComponentHandle<Component::FixedResult> fixedResult, ComponentHandle<Component::MotionResult> motionResult);

float3 GetLocalOffset(float3 worldOffset, float3 rotation);

float3 GetWorldOffset(float3 localOffset, float3 rotation);

Entity GetCamera(Chunk& a_chunk);