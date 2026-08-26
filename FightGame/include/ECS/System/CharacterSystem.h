#pragma once
#include "Components.h"
#include "Chunk.h"
#include "SystemContext.h"
#include "ISystemResponse.h"
#include "SystemAssist.h"
#include "AI/AIManager.h"


void HitPointSystem(Chunk& a_chunk, SystemContext& a_context);

// 接地判定
void GroundedSystem(Chunk& a_chunk, const SystemContext& a_context);

// キャラクターの可能な行動を定義する
void CharacterActionMaskSystem(Chunk& a_chunk, const SystemContext& a_context);

// 攻撃アクションステートの管理
void AttackActionSystem(Chunk& a_chunk, const SystemContext& a_context);

// 攻撃待機アクションステートの管理
void AttackWaitActionSystem(Chunk& a_chunk, const SystemContext& a_context);

// 攻撃開始のタメが終了した際、実際に攻撃を生成する
void AttackStartupSystem(Chunk& a_chunk, const SystemContext& a_context);

// 攻撃入力がされた際のタメ開始、タメがない場合攻撃生成
void CharacterAttackSystem(Chunk& a_chunk, const SystemContext& a_context);

// 攻撃Entityの終了確認
void AttackInstanceEndCheckSystem(Chunk& a_chunk, const SystemContext& a_context);

// 攻撃終了時をAIに通知する
void AttackInstanceResolveSystem(Chunk& a_chunk, const SystemContext& a_context, AIManager& a_aiManager);

// ジャンプによる物理への加算
void JumpPhysicsSystem(Chunk& a_chunk, const SystemContext& a_context);

// ガードする
void GuardSystem(Chunk& a_chunk, const SystemContext& a_context);

// ガードアクションステートの管理
void GuardActionSystem(Chunk& a_chunk, const SystemContext& a_context);

// ノックバックステートの管理
void KnockbackSystem(Chunk& a_chunk, const SystemContext& a_context);

void KnockbackStateSystem(Chunk& a_chunk, const SystemContext& a_context);

// !!!New!!!
void LookOnStateSystem(Chunk& a_chunk, const SystemContext& a_context);

// 入力を確定する
void MoveInputResolveSystem(Chunk& a_chunk, const SystemContext& a_context, AIManager& a_aiManager);

// 被弾解決
void AttackHitSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& a_response, AIManager& a_aiManager);

void AttackHitRecordCleanupSystem(Chunk& a_chunk, const SystemContext& a_context);

void EntryActionSystem(Chunk& a_chunk, const SystemContext& a_context);
