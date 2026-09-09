#include "AI/RunPlayerNode.h"

// !!!New!!!
RunPlayerNode::RunPlayerNode()
{
}

RunPlayerNode::~RunPlayerNode()
{
}

Node::Status RunPlayerNode::Tick(AIContext& context, const AIBlackboard& blackBord, AIMind& mind, const AISystemInfo& systemInfo, AIResult& result)
{
		// àÍìxèâä˙âª
		result.MoveDir = float2(0.0f, 0.0f);
		result.IsMove = false;
		result.UseAttack = false;
		result.AttackIndex = 0;
		result.Magnitube = 0.0f;
		result.UseGuard = false;
	
		// ÉvÉåÉCÉÑÅ[Ç™éÄÇÒÇ≈Ç¢ÇΩèÍçáé∏îs
		if (blackBord.isPlayerDead)
		{
			return NODE_FAILURE;
		}
	
		// é©êgÇ™ìoò^Ç≥ÇÍÇƒÇ¢Ç»Ç©Ç¡ÇΩèÍçáé∏îs
		const auto enemyIt = blackBord.enemyDatas.find(context.agentEntity);
		if (enemyIt == blackBord.enemyDatas.end())
		{
			return NODE_FAILURE;
		}
	
		// ìGÉfÅ[É^ÇéÊìæ
		const AIBlackboard::BBEnemyData& enemy = enemyIt->second;
	
		const float2 toPlayer(
			blackBord.playerPos.x - enemy.position.x,
			blackBord.playerPos.y - enemy.position.y);
	
		const float distSq = GetLengthSq(toPlayer);
		const float dist = GetLength(toPlayer);
		// ãóó£Çê≥ãKâª
		const float2 dir = Normalize(toPlayer);
	
		// ÉvÉåÉCÉÑÅ[ÇÃãtï˚å¸Ç÷à⁄ìÆ
		if (enemy.canMove && distSq > 0.0001f)
		{
			result.MoveDir = float2(dir.x * -1.0f, dir.y * -1.0f);
			result.IsMove = true;
			result.Magnitube = 1.0f;
		}
	
		// í«ê’åpë±
		if (!enemy.canMove && !enemy.canAttack)
		{
			return NODE_FAILURE;
		}
	
		return NODE_RUNNING;

}
