#pragma once
#include <memory>
#include <queue>

#include "IScene.h"
#include "World.h"
class GameScene : public IScene
{
public:
	GameScene(
		IModelCacheAcquisition& modelCache,
		IUICacheAcquisition& uiCache,
		std::function<void(std::string)> a_sceneChangeRequest,
		std::function<void()> a_gameEnd,
		std::function<void()> a_resetFPSRequest,
		std::function<void(bool)> a_setFixedCursor,
		Input& a_input
	);
	~GameScene() = default;
	void AdvanceUpdate(float dt) override;
	void Update() override;
	void Draw() override;
	void PostFrameProcess() override;

private:

	enum GameSceneState
	{
		GAMESCENE_DEFAULT,
		GAMESCENE_IMGUI,
		GAMESCENE_TITLE,
		GAMESCENE_TUTORIAL,
		GAMESCENE_END
	};

	GameSceneState state;
	void ChangeState();
	void ChangeWorld();
	bool isMouseLock;

	std::queue<std::string> worldChangeRequests;
	std::queue<int> tutorialRequests;
	std::unique_ptr<World> world;

	bool SetWorld(std::string);

	void TutorialRequest(int);
	void WorldRequest(std::string);

	IModelCacheAcquisition& modelCache;
	IUICacheAcquisition& uiCache;

	Input& input;

	//! シーン切り替え用のリクエスト
	std::function<void(std::string)> sceneChangeRequest;
	//! FPSリセット用のリクエスト
	std::function<void(void)> resetFPSRequest;
	//! ゲーム終了のコールバック
	std::function<void(void)> gameEnd;
	//! カーソル固定状態の変更リクエスト
	std::function<void(bool)> setFixedCursor;
};

