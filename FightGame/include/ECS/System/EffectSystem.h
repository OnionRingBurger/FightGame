#pragma once
#include "Chunk.h"
#include "SystemContext.h"

void FadeUISystem(Chunk& a_chunk, const SystemContext& a_context);

void CreateEffectSystem(Chunk& a_chunk, const SystemContext& a_context);

void TrailSystem(Chunk& a_chunk, const SystemContext& a_context);

void UIMoveSystem(Chunk& a_chunk, const SystemContext& a_context);

void UVMoveSystem(Chunk& a_chunk, const SystemContext& a_context);

void HPGaugeSystem(Chunk& a_chunk, const SystemContext& a_context);

void SpriteAnimationSystem(Chunk& a_chunk, const SystemContext& a_context);

void SpawnEfkEffectAreaSystem(Chunk& a_chunk, const SystemContext& a_context);

void SpawnEfkEffectSystem(Chunk& a_chunk, const SystemContext& a_context);

void UpdateEfkEffectSystem(Chunk& a_chunk, const SystemContext& a_context);

void UILerpSystem(Chunk& a_chunk, const SystemContext& a_context);

void StartUISystem(Chunk& a_chunk, const SystemContext& a_context);

void SoundSystem(Chunk& a_chunk, const SystemContext& a_context);