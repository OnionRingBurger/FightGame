#pragma once

constexpr float kTonnelWidth = 200.0f;
constexpr float kTonnelHeight = 120.0f;
constexpr float kTonnelColliderLength = 600.0f;
constexpr float kTonnelLength = 10000.0f;


constexpr float2 TitleButtonSize = { 0.5f, 0.2f };

constexpr float kTutorialTextTime = 35.0f;

constexpr float3 kGameStartLarpStartPos = {0.0f, 25.0f, -38.0f};
constexpr float3 kGameStartLarpEndPos = { 0.0f, 10.0f, -10.0f };
constexpr float kGameStartTime = 180.0f;

constexpr float3 kDefaultWorldPosition = { 0.0f, 50.0f, 50.0f };
constexpr float3 kClearWorldPosition = { 0.0f, 50.0f + 30.0, 50.0f };

constexpr float3 kDebugWorldSize = float3(48.0, 28.0f, 20.0f);

constexpr int kSectorVertexCount = 15;
constexpr float kWallThickness = 0.02f;
constexpr float kWallHitboxThickness = 0.2f;

// constexpr float kLookOnChangeCooltime = 15.0f;
constexpr float kLookOnChangeCooltime = 35.0f;
constexpr float kLookOnChangeMouseLength = 15.0f;

constexpr float kSectorScaleFlashTime = 15.0f;
constexpr float3 kSectorDefaultColor = float3(0.0f, 0.0f, 0.0f);
constexpr float3 kSectorFlashColor = float3(0.0f, 0.0f, 0.0f);

constexpr float kDefaultAttackHitCooldown = 300.0f;

constexpr float2 kPlayerMoveSpeed = float2(0.09f, 0.09f);
constexpr const char* kHitEffect = "IcicleBrokenChange.efkefc";
constexpr const char* kSpawnEffect = "Spawn.efk";
constexpr const char* kSnowEffect = "Snow.efkefc";
constexpr const char* kEntryEffect = "Entry.efkefc";
constexpr const char* kGuardEffectKey = "Guard.efk";
constexpr const char* kRedBossEntryKey = "RedBossEntry.efkefc";
constexpr const char* kDeadBrokenEffectKey = "DeadBroken.efkefc";
constexpr const char* kBossDeadBrokenEffectKey = "BossDeadBroken.efkefc";
constexpr const char* kPlayerHealKey = "Heal.efkefc";


// GhostShader: カメラからの距離フェード（exp(-d/L) の L）
constexpr float kGhostFadeRange = 20.0f;
