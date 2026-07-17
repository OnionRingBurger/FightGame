#pragma once
////////////////////////////////////////////////////
// 課題や気になったものなどを番外で実装するシステム
////////////////////////////////////////////////////

#include "BulletSystem.h"
#include "Components.h"
#include "Sound.h"
#include "SystemAssist.h"

using namespace Component;

void ResetConsoleSystem(Chunk& a_chunk, const SystemContext& a_context);

void SpawnObjectSystem(Chunk& a_chunk, const SystemContext& a_context);

void TheWorldSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_systemResponse);

void SetFireTypeSystem(Chunk& a_chunk, const SystemContext& a_context);




