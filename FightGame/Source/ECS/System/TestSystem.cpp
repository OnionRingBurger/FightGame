#include "TestSystem.h"
#include "Components.h"
#include "SystemAssist.h"
#include "DebugConsole.h"

using namespace Component;

void TestSystem(Chunk& a_chunk, const SystemContext& a_context, ISystemResponse& response, AIManager& aiManager, ComponentsSerialize& serialize)
{
	static bool isTest = false;
	if (!isTest)
	{
		isTest = true;
		NewEnemySpawn(serialize, a_chunk, aiManager, "Stage1Phase1");
		DebugConsole::SetDrawPos(15, 10);
		std::cout << "Stage1Phase1" << std::endl;
	}
}
