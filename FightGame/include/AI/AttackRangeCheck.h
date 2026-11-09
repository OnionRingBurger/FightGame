#pragma once

#include "AIData.h"
#include "MathAssist.h"

bool IsTargetInAttackRange(
	float2 targetPos,
	float2 attackerPos,
	float attackerYaw,
	const AIBlackboard::BBAttackData& attack);
