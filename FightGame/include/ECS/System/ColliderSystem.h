#pragma once
#include "Chunk.h"
#include "SystemContext.h"
#include "ISystemResponse.h"

// OBB‚Ì“–‚½‚è”»’è‚ğ¶¬‚·‚é
void ColliderSystem(Chunk& a_chunk, const SystemContext& a_context);

// OBB‚Ì”»’è‚ğˆ—‚µŒ‹‰Ê‚ğ•Ô‚·
void ColliderCheckSystem(Chunk& a_chunk, const SystemContext& a_context);

// “–‚½‚è”»’è‚É‚æ‚é‰Ÿ‚µ–ß‚µ‚ğPose‚É‘‚«‚Ş
void ColliderBackSystem(Chunk& a_chunk, const SystemContext& a_context);

// ƒŒƒC‚Ì”»’è‚ğˆ—‚µŒ‹‰Ê‚ğ•Ô‚·
void RaySystem(Chunk& a_chunk, const SystemContext& a_context);

// ‰~Œ`‚Ì”»’è‚ğˆ—‚µŒ‹‰Ê‚ğ•Ô‚·
void SectorCheckSystem(Chunk& a_chunk, const SystemContext& a_context);