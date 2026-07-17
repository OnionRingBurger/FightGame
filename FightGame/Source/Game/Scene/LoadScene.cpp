#include "LoadScene.h"
#include "DebugConsole.h"
#include "LoadWorld.h"
#include <unordered_set>

LoadScene::LoadScene(
	std::function<void(std::string)> a_sceneChange,
	ThreadSafeQueue<ModelLoadJob>& a_modelJobQueue,
	ThreadSafeQueue<TextureLoadJob>& a_textureJobQueue,
	std::vector<modelLoadData> a_modelDatas,
	std::vector<std::string> a_textureDatas,
	IModelCacheAcquisition& a_modelCache,
	IUICacheAcquisition& a_uiCache,
	Input& a_input)
	: sceneChange(a_sceneChange)
	, inited(false)
	, modelJobQueue(a_modelJobQueue)
	, textureJobQueue(a_textureJobQueue)
	, modelDatas(a_modelDatas)
	, textureDatas(a_textureDatas)
{
	DebugConsole::ToggleConsole();

	std::vector<modelLoadData> useModelDatas = {
	};

	std::vector<std::string> useTextureDatas = {
		"LoadMainCar",
		"LoadRail",
		"LoadWind"
	};

	for (auto it : useModelDatas)
	{
		std::shared_ptr<std::atomic_bool> endFlag = std::make_shared<std::atomic_bool>(false);
		a_modelJobQueue.Push(std::move(ModelLoadJob(it.first, it.second, endFlag)));
		useDataLoadEndFlags.push_back(endFlag);
	}

	for (auto it : useTextureDatas)
	{
		std::shared_ptr<std::atomic_bool> endFlag = std::make_shared<std::atomic_bool>(false);
		a_textureJobQueue.Push(std::move(TextureLoadJob(it, endFlag)));
		useDataLoadEndFlags.push_back(endFlag);
	}


	world = std::make_unique<LoadWorld>(a_modelCache, a_uiCache, a_input);
	world->InitWorld();
	// ロード量を保持
	maxLoadCount = loadEndFlags.size();

}

void LoadScene::AdvanceUpdate(float dt)
{
	world->ContextUpdate(dt);
}

void LoadScene::Update()
{

	RemoveIfFlaged(useDataLoadEndFlags);
	if (useDataLoadEndFlags.empty() && !inited)
	{
		inited = true;

		// 各モデル、テクスチャをロード予約して終了フラグを保持
		for (auto it : modelDatas)
		{
			std::shared_ptr<std::atomic_bool> endFlag = std::make_shared<std::atomic_bool>(false);
			modelJobQueue.Push(std::move(ModelLoadJob(it.first, it.second, endFlag)));
			loadEndFlags.push_back(endFlag);
		}

		for (auto it : textureDatas)
		{
			std::shared_ptr<std::atomic_bool> endFlag = std::make_shared<std::atomic_bool>(false);
			textureJobQueue.Push(std::move(TextureLoadJob(it, endFlag)));
			loadEndFlags.push_back(endFlag);
		}
	}



	if (!inited) return;
	RemoveIfFlaged(loadEndFlags);
	if (loadEndFlags.empty())
	{
		DebugConsole::ToggleConsole();
		sceneChange("Game");
	}
	world->UpdateWorld();
}

void LoadScene::Draw()
{
	if (!inited) return;
	world->DrawWorld();
}

void LoadScene::PostFrameProcess()
{
}

void LoadScene::RemoveIfFlaged(std::vector<std::shared_ptr<std::atomic_bool>>& loadEndFlags)
{
	for (int i = loadEndFlags.size() - 1; i >= 0; i--)
	{
		if (loadEndFlags.at(i)->load())
		{
			loadEndFlags[i] = std::move(loadEndFlags.back());
			loadEndFlags.pop_back();
		}
	}
}

