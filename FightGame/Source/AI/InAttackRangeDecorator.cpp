#include "AI/InAttackRangeDecorator.h"

#include "MathAssist.h"
#include "DebugConsole.h"

// !!!New!!!
InAttackRangeDecorator::InAttackRangeDecorator(std::unique_ptr<Node> child)
	: DecoratorNode(std::move(child))
{
}

InAttackRangeDecorator::~InAttackRangeDecorator()
{
}

bool InAttackRangeDecorator::CheckCondition(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result)
{
	(void)mind;
	(void)systemInfo;
	(void)result;

	// TODO ‹°‚ç‚­”ÍˆÍŠO‚Å‚à’Ê‚µ‚Ä‚µ‚Ü‚¤ƒoƒO‚ª‚ ‚é–Í—lA‚ ‚é‚¢‚Í‰º‚ÌAttack‘¤‚ª‚¨‚©‚µ‚¢‚©‚à

	// ƒvƒŒƒCƒ„[‚ªŽ€‚ñ‚Å‚¢‚½ê‡”²‚¯‚é
	if (blackBord.isPlayerDead)
	{
		return false;
	}

	// Ž©g‚ÌBBî•ñ‚ðŽæ“¾‚·‚é
	const auto enemyIt = blackBord.enemyDatas.find(context.agentEntity);
	if (enemyIt == blackBord.enemyDatas.end())
	{
		return false;
	}

	const AIBlackboard::BBEnemyData& enemy = enemyIt->second;
	if (enemy.attackDatas.empty())
	{
		return false;
	}

	// Ž–‘O‚É—v‘f‚ðŒvŽZ‚·‚é
	const float2 toPlayer(
		blackBord.playerPos.x - enemy.position.x,
		blackBord.playerPos.y - enemy.position.y);
	const float dist = GetLength(toPlayer);
	float2 nomalizeVec = Normalize(toPlayer);
	float2 enemyFrontVec = AngleToVector(-enemy.rotation + 90.0f);
	float dot = nomalizeVec.x * enemyFrontVec.x + nomalizeVec.y * enemyFrontVec.y;
	float toPlayerAngle = acosf(dot) * DEG;

	// UŒ‚”ÍˆÍ‚ð‘–¸‚µAˆê‚Â‚Å‚à”ÍˆÍ“à‚É‚ ‚ê‚Î¬Œ÷‚ð•Ô‚·
	for (const AIBlackboard::BBAttackData& attack : enemy.attackDatas)
	{
		float halfAttackAngle = attack.angle * 0.5f;

		// ŽË’ö‹——£ŠO‚¾‚Á‚½ê‡”²‚¯‚é
		if (!(dist >= attack.minLength) || !(dist <= attack.maxLength))
		{
			continue;
		}

		// UŒ‚Šp‚æ‚èŠO‚É‚¢‚½ê‡ê‡”²‚¯‚é

		if (halfAttackAngle / 5.0f < toPlayerAngle)
		{
			continue;
		}

		// “G‚ªUŒ‚”ÍˆÍ“à‚É‚¢‚é‚½‚ßAŠÖ”‚ðI—¹‚µ¬Œ÷‚ð•Ô‚·
		return true;
	}

	// “G‚ªUŒ‚”ÍˆÍ“à‚É‚¢‚È‚©‚Á‚½‚½‚ßŽ¸”s
	return false;
}
