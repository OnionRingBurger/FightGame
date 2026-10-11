#include "LoadScene.h"
#include "DebugConsole.h"
#include "LoadWorld.h"
#include <unordered_set>
#include "Defines.h"

LoadScene::LoadScene(
	std::function<void(std::string)> a_sceneChange,
	LoadJobQueues& a_jobQueues,
	LoadDatas a_loadDatas,
	IModelCacheAcquisition& a_modelCache,
	IUICacheAcquisition& a_uiCache,
	IEffectCacheAcquisition& effectCache,
	Input& a_input,
	ComponentsSerialize& a_serialize,
	PrefabManager& a_prefabManager)
	: sceneChange(a_sceneChange)
	, inited(false)
	, jobQueues(a_jobQueues)
	, uiCache(a_uiCache)
	, loadDatas(std::move(a_loadDatas))
	
{
	std::vector<modelLoadData> useModelDatas = {
	};

	std::vector<std::string> useTextureDatas = {
		"LoadSprite",

	};

	for (auto it : useModelDatas)
	{
		std::shared_ptr<std::atomic_bool> endFlag = std::make_shared<std::atomic_bool>(false);
		jobQueues.model.Push(std::move(ModelLoadJob(it.first, it.second, endFlag)));
		useDataLoadEndFlags.push_back(endFlag);
	}

	for (auto it : useTextureDatas)
	{
		std::shared_ptr<std::atomic_bool> endFlag = std::make_shared<std::atomic_bool>(false);
		jobQueues.texture.Push(std::move(TextureLoadJob(it, endFlag)));
		useDataLoadEndFlags.push_back(endFlag);
	}

	for (auto it : loadDatas.textFormat)
	{
		std::shared_ptr<std::atomic_bool> endFlag = std::make_shared<std::atomic_bool>(false);
		jobQueues.textFormat.Push(std::move(TextFormatLoadJob(it.key, it.useFile, it.font, it.fontFile, it.size, endFlag)));
		useDataLoadEndFlags.push_back(endFlag);
	}

	world = std::make_unique<LoadWorld>(a_modelCache, a_uiCache, effectCache, a_input, a_serialize, a_prefabManager);
	world->InitWorld();
	// ロード量を保持
	maxLoadCount = (int)loadEndFlags.size();

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
		// !!!New!!!
		for (const auto& it : loadDatas.textUI)
		{
			if (uiCache.GetTextFormat(it.formatKey) == nullptr)
			{
				return;
			}
		}

		inited = true;

		// 各モデル、テクスチャをロード予約して終了フラグを保持
		for (auto it : loadDatas.model)
		{
			std::shared_ptr<std::atomic_bool> endFlag = std::make_shared<std::atomic_bool>(false);
			jobQueues.model.Push(std::move(ModelLoadJob(it.first, it.second, endFlag)));
			loadEndFlags.push_back(endFlag);
		}

		for (auto it : loadDatas.texture)
		{
			std::shared_ptr<std::atomic_bool> endFlag = std::make_shared<std::atomic_bool>(false);
			jobQueues.texture.Push(std::move(TextureLoadJob(it, endFlag)));
			loadEndFlags.push_back(endFlag);
		}

		for (auto it : loadDatas.effect)
		{
			std::shared_ptr<std::atomic_bool> endFlag = std::make_shared<std::atomic_bool>(false);
			jobQueues.effect.Push(std::move(EffectLoadJob(it.key, it.magnificent, endFlag)));
			loadEndFlags.push_back(endFlag);
		}

		for (auto it : loadDatas.textUI)
		{
			std::shared_ptr<std::atomic_bool> endFlag = std::make_shared<std::atomic_bool>(false);
			// !!!New!!!
			ComPtr<IDWriteTextFormat> format = uiCache.GetTextFormat(it.formatKey);
			jobQueues.textUI.Push(std::move(TextUILoadJob(it.key, it.text, it.formatKey, it.width, it.height, endFlag, format)));
			loadEndFlags.push_back(endFlag);
		}

		for (auto it : loadDatas.jsonPrefab)
		{
			std::shared_ptr<std::atomic_bool> endFlag = std::make_shared<std::atomic_bool>(false);
			jobQueues.jsonPrefab.Push(std::move(JsonPrefabLoadJob(endFlag, it.key)));
			useDataLoadEndFlags.push_back(endFlag);
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
	for (int i = (int)loadEndFlags.size() - 1; i >= 0; i--)
	{
		if (loadEndFlags.at(i)->load())
		{
			loadEndFlags[i] = std::move(loadEndFlags.back());
			loadEndFlags.pop_back();
		}
	}
}

