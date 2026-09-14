#include "AI/AttackRangeCheck.h"

#include "SystemAssist.h"

bool IsTargetInAttackRange(
	float2 targetPos,
	float2 attackerPos,
	float attackerYaw,
	const AIBlackboard::BBAttackData& attack)
{
	const float3 worldOffset = GetLocalOffset(attack.followOffset, float3(0.0f, attackerYaw, 0.0f));
	const float2 sectorOrigin(
		attackerPos.x + worldOffset.x,
		attackerPos.y + worldOffset.z);

	const float2 diff(
		targetPos.x - sectorOrigin.x,
		targetPos.y - sectorOrigin.y);
	const float dist = GetLength(diff);
	if (dist < attack.minLength || dist > attack.maxLength)
	{
		return false;
	}

	const float3 forward3 = GetForward(float3(0.0f, attackerYaw, 0.0f));
	const float2 forward(forward3.x, forward3.z);
	const float halfAttackAngle = attack.angle * 0.5f;
	float allowedHalfAngle = halfAttackAngle;
	if (halfAttackAngle < 180.0f)
	{
		allowedHalfAngle = halfAttackAngle / 5.0f;
	}
	const float cosHalfAngle = cosf(allowedHalfAngle * RAD);

	if (dist > 1e-5f)
	{
		const float2 normDiff = Normalize(diff);
		const float cosTheta = normDiff.x * forward.x + normDiff.y * forward.y;
		if (cosTheta < cosHalfAngle)
		{
			return false;
		}
	}

	return true;
}
