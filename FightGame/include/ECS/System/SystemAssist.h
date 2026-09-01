#pragma once
#include "Chunk.h"
#include "SystemContext.h"
#include "MathAssist.h"
#include "Components.h"
#include "ComponentsSerialize.h"
#include "AIManager.h"

////////////////////////////////////////////////////
// システム内で使うための汎用昨日
////////////////////////////////////////////////////



Entity GetPlayer(Chunk& a_chunk, float3 a_position);

Entity GetCameraRig(Chunk& a_chunk);

Entity GetRail(Chunk& a_chunk);

Entity GetLookOnMarker(Chunk& a_chunk);

float3 GetEntityWorldPos(Chunk& a_chunk, Entity a_entity);


// Pose 優先、無ければ Position。遅延系座標処理でのみ使う
float3 GetEntityPosePos(Chunk& a_chunk, Entity a_entity);

// !!!New!!! Pose 優先、無ければ Rotation（pitch/yaw/roll）
float3 GetEntityPoseRot(Chunk& a_chunk, Entity a_entity);

void ResetFixedPosState(ComponentHandle<Component::FixedResult> fixedResult, ComponentHandle<Component::MotionResult> motionResult);

float3 GetLocalOffset(float3 worldOffset, float3 rotation);

float3 GetWorldOffset(float3 localOffset, float3 rotation);

Entity GetCamera(Chunk& a_chunk);

// 対象Entityが指定したActionを使用可能な状態か確認する
bool IsActionAllowed(Chunk& a_chunk, Entity a_entity, Component::ActionFlag a_flag);

void CancelPlayerAttackIfAble(Chunk& a_chunk, Entity a_entity);

void NewEnemySpawn(ComponentsSerialize& a_serialize,Chunk& a_chunk, AIManager& aiManager, std::string a_newSceneName);

void RegisterAllAttackSectorsFromAttackData(int a_circumferenceCount);

void SafeDeleteEffectEntity(Chunk& a_chunk, Entity a_effect);