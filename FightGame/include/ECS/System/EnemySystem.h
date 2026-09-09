#pragma once
#include "Chunk.h"
#include "SystemContext.h"
#include "ISystemResponse.h"
#include "GameData.h"
#include "MathAssist.h"
#include "AIManager.h"

struct PlayerHitViewInfo
{
	bool isOffView = false;
	float2 screenDirection = float2(); // 画面XY上の単位ベクトル
};

// 被弾元が視線から外れているかと、画面から見た被弾元の方向を返す
PlayerHitViewInfo EvaluatePlayerHitView(Chunk& a_chunk, Entity a_player, Entity a_hitEntity, float a_minAngleDeg);

// プレイヤーの視線と被弾元への方向のなす角が a_minAngleDeg 以上か
bool IsPlayerHitFromOffView(Chunk& a_chunk, Entity a_player, Entity a_hitEntity, float a_minAngleDeg);

void EnemyDeadSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_response);

void BossDeadSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_response);

void EnemyBulletDeadSystem(Chunk& a_chunk, const SystemContext& a_context);




