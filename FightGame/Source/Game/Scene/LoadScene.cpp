#include "LoadScene.h"
#include "DebugConsole.h"
#include "LoadWorld.h"
#include <unordered_set>
#include "Defines.h"

LoadScene::LoadScene(
	std::function<void(std::string)> a_sceneChange,
	ThreadSafeQueue<ModelLoadJob>& a_modelJobQueue,
	ThreadSafeQueue<TextureLoadJob>& a_textureJobQueue,
	ThreadSafeQueue<EffectLoadJob>& a_effectJobQueue,
	std::vector<modelLoadData> a_modelDatas,
	std::vector<std::string> a_textureDatas,
	std::vector<effectLoadData> a_effectDatas,
	IModelCacheAcquisition& a_modelCache,
	IUICacheAcquisition& a_uiCache,
	IEffectCacheAcquisition& effectCache,
	Input& a_input,
	ComponentsSerialize& a_serialize)
	: sceneChange(a_sceneChange)
	, inited(false)
	, modelJobQueue(a_modelJobQueue)
	, textureJobQueue(a_textureJobQueue)
	, effectJobQueue(a_effectJobQueue)
	, modelDatas(a_modelDatas)
	, textureDatas(a_textureDatas)
	, effectDatas(a_effectDatas)
	
{
	// DebugConsole::ToggleConsole();

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


	world = std::make_unique<LoadWorld>(a_modelCache, a_uiCache, effectCache, a_input, a_serialize);
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

		for (auto it : effectDatas)
		{
			std::shared_ptr<std::atomic_bool> endFlag = std::make_shared<std::atomic_bool>(false);
			effectJobQueue.Push(std::move(EffectLoadJob(it.key, it.magnificent, endFlag)));
			loadEndFlags.push_back(endFlag);
		}
	}

	if (!inited) return;
	RemoveIfFlaged(loadEndFlags);
	if (loadEndFlags.empty())
	{
#ifdef CREATEGAME
		sceneChange("CreateGame");
#else
		sceneChange("Game");
		
#endif // CREATEGAME

		
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

