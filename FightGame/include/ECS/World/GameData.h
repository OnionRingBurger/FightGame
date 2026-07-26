#pragma once

constexpr float kTonnelWidth = 200.0f;
constexpr float kTonnelHeight = 120.0f;
constexpr float kTonnelColliderLength = 600.0f;
constexpr float kTonnelLength = 10000.0f;

// ループするトンネル区間（GameWorld のトンネル生成と対応）
constexpr int kTonnelSegmentCount = 6;
constexpr float kTonnelSegmentLength = 60.0f;
constexpr float kTonnelWarpForwardMargin = 0.2f;
constexpr float kTonnelLoopDepth = (kTonnelSegmentCount - 1) * kTonnelSegmentLength;

constexpr float kEnemyMoveSpeed = 0.2f;

constexpr int kEnemyRandomWidth = (int)kTonnelWidth / 10;
constexpr int kEnemyRandomHeight = (int)kTonnelHeight / 10;
constexpr int kEnemyRandomDepth = (int)kTonnelLoopDepth / 30;
constexpr float kEnemyRandomRate = 1.6f;

constexpr float kEnemySpawnRateWidth = (kTonnelWidth / 2.0f - (float)kEnemyRandomWidth * kEnemyRandomRate / 2.0f) * 0.7f;
constexpr float kEnemySpawnRateHeight = (kTonnelHeight / 2.0f - (float)kEnemyRandomHeight * kEnemyRandomRate / 2.0f) * 0.7f;
constexpr float kEnemySpawnRateDepth = (kTonnelLoopDepth / 2.0f - (float)kEnemyRandomDepth * kEnemyRandomRate / 2.0f) * 0.7f;

constexpr float kEnemyFlyPower = 1.8f;
constexpr float kEnemyNormalScale = 1.0f;
constexpr float3 kEnemyShooterScale = { 1.0f, 1.0f, 1.0f };
constexpr float kEnemyAttackDamage = 45.0f;
constexpr float kEnemyShotDamage = 45.0f;
constexpr float kEnemyMaxHP = 3.0f;
constexpr float kDefaultAttackHitCooldown = 300.0f;
constexpr float3 kDefaultWorldPosition = {0.0f, 50.0f, 0.0f};

constexpr float kEnemyShotSpeed = 0.8f;

// 視線と被弾元方向のなす角がこの値以上なら「視線から外れた被弾」
constexpr float kDamageDirectionMinAngleDeg = 45.0f;

//! TitleWorld のトンネル
constexpr float kTitleTonnelWidth = 60.0f;
constexpr float kTitleTonnelHeight = 60.0f;
constexpr float kTitleTonnelMeshSize = 60.0f;
constexpr float kTitleTonnelSegmentLength = 60.0f;
constexpr float kTitleTonnelFloorDepth = 60.5f;
constexpr int kTitleTonnelSegmentCount = 5;
constexpr float kTitleTonnelWarpMargin = 0.2f;
constexpr float kRailPosition = kTonnelHeight / 2.5f;

//! 画面に描画するための座標
constexpr float kMineCarModelPosition = -15.0f;
constexpr float kRailModelPosition = -kTonnelSegmentLength * 1.5f + kRailPosition -15.0f;

// GhostShader: カメラからの距離フェード（exp(-d/L) の L）
constexpr float kGhostFadeRange = 20.0f;