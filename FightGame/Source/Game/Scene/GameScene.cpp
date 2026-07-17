#include "GameScene.h"

#include "TitleWorld.h"
#include "GameWorld.h"
#include "ClearWorld.h"
#include "ResultWorld.h"
#include "TestWorld.h"
#include "ProtoWorld.h"

constexpr const char* kStartWorld = "Proto";

GameScene::GameScene(
	IModelCacheAcquisition& a_modelCache,
	IUICacheAcquisition& a_uiCache,
	std::function<void(std::string)> a_sceneChangeRequest,
	std::function<void()> a_gameEnd,
	std::function<void()> a_resetFPSRequest,
	Input& a_input)
	: state(GAMESCENE_DEFAULT)
	, isMouseLock(false)
	, sceneChangeRequest(a_sceneChangeRequest)
	, modelCache(a_modelCache)
	, uiCache(a_uiCache)
	, input(a_input)
	, gameEnd(a_gameEnd)
	, resetFPSRequest(a_resetFPSRequest)
{

	SetWorld(kStartWorld);

	input.RegisterKey("GameEnd", VK_ESCAPE);
	input.RegisterButton("GameEnd", VK_PAD_START);
	input.RegisterKey("IMGUI", VK_F2);
	input.RegisterKey("TutorialSkip", VK_RETURN);
}

void GameScene::AdvanceUpdate(float dt)
{
	world->ContextUpdate(dt);
}

void GameScene::Update()
{
	world->UpdateWorld();
	if (input.IsRegisterTrigger("GameEnd"))
	{
		gameEnd();
	}
}

void GameScene::Draw()
{
	world->DrawWorld();
}

void GameScene::PostFrameProcess()
{
	ChangeState();
	ChangeWorld();
}

void GameScene::ChangeState()
{

	switch (state)
	{
	case GAMESCENE_DEFAULT:
		if (!tutorialRequests.empty())
		{
			tutorialRequests.pop();
			world->ChangeState(WAIT);
			state = GAMESCENE_TUTORIAL;
			DebugConsole::ToggleConsole();
			std::cout << "チュートリアル起動！" << std::endl;
		}
		else if(input.IsRegisterTrigger("IMGUI"))
		{
#ifdef _DEBUG

			state = GAMESCENE_IMGUI;
			world->ChangeState(IMGUI);

#endif // _DEBUG
		}


		break;
	case GAMESCENE_IMGUI:
		state = GAMESCENE_DEFAULT;
		world->ChangeState(USECHUNK);
		break;

	case GAMESCENE_TUTORIAL:
		if (input.IsRegisterTrigger("TutorialSkip"))
		{
			state = GAMESCENE_DEFAULT;
			world->ChangeState(USECHUNK);
			DebugConsole::ToggleConsole();
		}
		break;

	case GAMESCENE_END:
		sceneChangeRequest("End");
		break;
	}	
}

void GameScene::ChangeWorld()
{
	while (!worldChangeRequests.empty())
	{
		std::string request = worldChangeRequests.front();
		worldChangeRequests.pop();
		// Worldを設定出来たら抜ける
		if (SetWorld(request))
		{
			// Queueは初期化する
			std::queue<std::string> empty;
			worldChangeRequests.swap(empty);
			state = GAMESCENE_DEFAULT;
			std::srand((unsigned)time(NULL));
			resetFPSRequest();
			break;
		}
	}

}

bool GameScene::SetWorld(std::string a_key)
{
	bool ret = true;
	if (a_key == "Title")
	{
		world = std::make_unique<TitleWorld>(
			modelCache,
			uiCache,
			[this](int a_id) {TutorialRequest(a_id); },
			[this](std::string a_key) {WorldRequest(a_key); },
			input
		);

		world->InitWorld();
	}
	else if (a_key == "Game")
	{
		world = std::make_unique<GameWorld>(
			modelCache,
			uiCache,
			[this](int a_id) {TutorialRequest(a_id); },
			[this](std::string a_key) {WorldRequest(a_key); },
			input
		);

		world->InitWorld();

	}
	else if (a_key == "Result")
	{
		world = std::make_unique<ResultWorld>(
			modelCache,
			uiCache,
			[this](int a_id) {TutorialRequest(a_id); },
			[this](std::string a_key) {WorldRequest(a_key);},
			input
		);

		world->InitWorld();
	}
	else if (a_key == "Clear")
	{
		world = std::make_unique<ClearWorld>(
			modelCache,
			uiCache,
			[this](int a_id) {TutorialRequest(a_id); },
			[this](std::string a_key) {WorldRequest(a_key); },
			input
		);

		world->InitWorld();
	}
	else if (a_key == "Test")
	{
		world = std::make_unique<TestWorld>(
			modelCache,
			uiCache,
			[this](int a_id) {TutorialRequest(a_id); },
			[this](std::string a_key) {WorldRequest(a_key); },
			input
		);

		world->InitWorld();
	}
	else if (a_key == "Proto")
	{
		world = std::make_unique<ProtoWorld>(
			modelCache,
			uiCache,
			[this](int a_id) {TutorialRequest(a_id); },
			[this](std::string a_key) {WorldRequest(a_key); },
			input
		);

		world->InitWorld();
	}
	else
	{
		ret = false;
	}

	return ret;
}

void GameScene::TutorialRequest(int a_id)
{
	tutorialRequests.push(a_id);
}

void GameScene::WorldRequest(std::string a_key)
{
	worldChangeRequests.push(a_key);
}
