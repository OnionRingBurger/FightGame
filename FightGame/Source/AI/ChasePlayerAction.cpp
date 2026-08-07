#include "AI/ChasePlayerAction.h"

#include <cmath>

#include "MathAssist.h"

// !!!New!!!
ChasePlayerAction::ChasePlayerAction()
{
}

ChasePlayerAction::~ChasePlayerAction()
{
}

Node::Status ChasePlayerAction::Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result)
{
	(void)systemInfo;
	(void)mind;
	// ˆê“x‰Šú‰»
	result.MoveDir = float2(0.0f, 0.0f);
	result.IsMove = false;
	result.UseAttack = false;
	// !!!New!!!
	result.AttackIndex = 0;

	// ƒvƒŒƒCƒ„[‚ªŽ€‚ñ‚Å‚¢‚½ê‡Ž¸”s
	if (blackBord.isPlayerDead)
	{
		return NODE_FAILURE;
	}

	// Ž©g‚ª“o˜^‚³‚ê‚Ä‚¢‚È‚©‚Á‚½ê‡Ž¸”s
	const auto enemyIt = blackBord.enemyDatas.find(context.agentEntity);
	if (enemyIt == blackBord.enemyDatas.end())
	{
		return NODE_FAILURE;
	}

	// “Gƒf[ƒ^‚ðŽæ“¾
	const AIBlackboard::BBEnemyData& enemy = enemyIt->second;

	const float2 toPlayer(
		blackBord.playerPos.x - enemy.position.x,
		blackBord.playerPos.y - enemy.position.y);

	const float distSq = GetLengthSq(toPlayer);
	// const float dist = GetLength(toPlayer);
	// ‹——£‚ð³‹K‰»
	const float2 dir = Normalize(toPlayer);

	// ƒvƒŒƒCƒ„[•ûŒü‚ÖˆÚ“®
	if (enemy.canMove && distSq > 0.0001f)
	{
		result.MoveDir = dir;
		result.IsMove = true;
	}

	// UŒ‚ƒf[ƒ^‚ª–³‚¢ê‡‚ÍUŒ‚‚¹‚¸’ÇÕ‚Ì‚Ý
	//if (!enemy.attackDatas.empty())
	//{
	//	// ‰¼: 0”Ô–Ú‚ÌUŒ‚ƒf[ƒ^‚ÅŽË’ö”»’è
	//	const AIBlackboard::BBAttackData& attack0 = enemy.attackDatas[0];
	//	const float minLen = attack0.minLength;
	//	const float maxLen = attack0.maxLength;
	//	const bool inRange = (dist >= minLen) && (dist <= maxLen);

	//	const float targetYaw = std::atan2(toPlayer.x, toPlayer.y) * DEG;
	//	const float yawDelta = std::fabs(DeltaDeg(enemy.rotation, targetYaw));
	//	// angle ‚Íî‚Ì‘SŠp‘z’èB•Ð‘¤‚Í”¼•ª
	//	const float halfAngle = attack0.angle * 0.5f;
	//	const bool facingPlayer = yawDelta <= halfAngle;

	//	if (enemy.canAttack && inRange && facingPlayer)
	//	{
	//		result.UseAttack = true;
	//		result.AttackIndex = 0;
	//		result.MoveDir = float2(0.0f, 0.0f);
	//		result.IsMove = false;
	//		return NODE_SUCCES;
	//	}
	//}

	// ’ÇÕŒp‘±
	if (!enemy.canMove && !enemy.canAttack)
	{
		return NODE_FAILURE;
	}

	return NODE_RUNNING;
}
