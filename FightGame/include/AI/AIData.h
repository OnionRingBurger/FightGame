#pragma once
#include <unordered_map>
#include <vector>
#include <string>

#include "MathAssist.h"
#include "ECSTypes.h"

// Worldの状態書き込み
struct AIBlackboard
{
	struct BBAttackData
	{
		// 攻撃範囲
		float minLength;
		float maxLength;
		float angle;
		float3 followOffset;
		// 威力、隙
		float damageValue;
		float waitTime;
		bool endWithOwnerAction = true;
	};

	struct BBEnemyData
	{
		// 座標系
		float2 position;
		float rotation;

		// 行動フラグ
		bool canMove;
		bool canAttack;

		// 攻撃データ
		std::vector<BBAttackData> attackDatas;

		// Role（性質・0?1つまみ）。技キットと同じ袋。モード変更時は一緒に書き換える
		float distFocus = 0.5f;
		float farnessPref = 0.5f;
		float hitSensitivity = 0.5f;
		float stanceStickiness = 0.5f;

		// !!!New!!!
		float distCoefficient = 1.0f;
		float farnessCoefficient = 1.0f;
		float hitCoefficient = 3.0f;
		float stanceCoefficient = 1.2f;

		// 各方針の継続時の累乗倍率
		float probeContinuePowRate = 1.5f;
		float holdContinuePowRate = 0.6f;
		float escapeContinuePowRate = 2.5f;

		// !!!New!!!
		bool profileReady = false;
	};

	float2 playerPos;
	bool isPlayerDead;

	std::unordered_map<Entity, BBEnemyData> enemyDatas;
};


// 各敵毎の状態書き込み用データ
struct AISenseEnemyFrame
{
	float2 position;
	float rotation = 0.0f;
	bool canMove = false;
	bool canAttack = false;
};

// 外部から受け取る状態書き込み用データ
struct AISenseFrame
{
	float2 playerPos;
	bool isPlayerDead = false;
	std::unordered_map<Entity, AISenseEnemyFrame> enemies;
};


// 各敵毎の設定書き込み用データ
struct AISenseEnemyProfile
{
	std::vector<AIBlackboard::BBAttackData> attackDatas;
	float distFocus = 0.5f;
	float farnessPref = 0.5f;
	float hitSensitivity = 0.5f;
	float stanceStickiness = 0.5f;
	float probeContinuePowRate = 1.5f;
	float holdContinuePowRate = 0.6f;
	float escapeContinuePowRate = 2.5f;
};

// 外部から受け取る設定書き込み用データ
struct AISenseProfile
{
	std::unordered_map<Entity, AISenseEnemyProfile> enemies;
};



// ノードの利用状況
struct AIContext
{
	std::string BTType;
	Entity agentEntity;
	std::vector<int> nodeIndexes;
	float actionDuration;
};

// システム情報（当面は空）
struct AISystemInfo
{
	AISystemInfo() = default;
};

// AI判断結果
struct AIResult
{
	// 移動方向
	float2 MoveDir = float2(0.0f, 0.0f);
	float Magnitube = 1.0f;
	bool IsMove = false;


	// 使用する攻撃番号
	int AttackIndex = 0;
	bool UseAttack = false;

	// ガード
	bool UseGuard = false;
};


// 敵の行動方針
enum class AIStance
{
	// 何もしない
	NoneStance = -1,
	// 不利な位置関係を脱する
	EscapeDisadvantage = 0,
	// 有利な位置を維持・活用する
	HoldAdvantage = 1,
	// ニュートラル様子見
	NeutralProbe = 2,
};

// 思考の状態と記憶
struct AIMind
{
	// 現在の方針
	AIStance stance = AIStance::HoldAdvantage;
	// 方針再評価用ポイント、最大値を超えると方針の変更を検討する
	float updatePoints = 0.0f;
	// 方針再評価用点の最大値
	float maxUpdatePoints = 120.0f;
	// 方針連続使用回数
	int stanceContinueCount = 0;
	// 攻撃連続使用
	int lastHitAttackIndex = 0;
	// 連続的中回数
	int sameAttackStreak = 0;
	// 現更新ループでの攻撃成功回数、失敗回数
	int attackSuccessCount = 0;
	int attackMissCount = 0;
	// 被弾数、ガード数
	int sameTakenCount = 0;
	int sameGuardCount = 0;
};

// ランタイムデータ
struct AIRuntime
{
	AIContext context;
	AIMind mind;
	AIResult result;
};