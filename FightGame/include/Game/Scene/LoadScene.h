#pragma once
#include "IScene.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <condition_variable>
#include <mutex>
#include "ThreadSafeQueue.h"
#include "Loading.h"
#include "World.h"

class LoadScene : public IScene
{
public:
	LoadScene(std::function<void(std::string)> a_sceneChange,
		ThreadSafeQueue<ModelLoadJob>& a_modelJobQueue, 
		ThreadSafeQueue<TextureLoadJob>& a_textureJobQueue,
		std::vector<modelLoadData> a_modelDatas,
		std::vector<std::string> a_textureDatas,
		IModelCacheAcquisition& a_modelCache,
		IUICacheAcquisition& a_uiCache,
		Input& a_input,
		ComponentsSerialize& a_serialize);

	~LoadScene() = default;
	void AdvanceUpdate(float dt) override;
	void Update() override;
	void Draw() override;
	void PostFrameProcess() override;

private:
	bool inited;
	std::vector<std::shared_ptr<std::atomic_bool>> useDataLoadEndFlags;
	std::function<void(std::string)> sceneChange;
	std::vector<std::shared_ptr<std::atomic_bool>> loadEndFlags;
	std::unique_ptr<World> world;
	int maxLoadCount;
	ThreadSafeQueue<ModelLoadJob>& modelJobQueue;
	ThreadSafeQueue<TextureLoadJob>& textureJobQueue;
	std::vector<modelLoadData> modelDatas;
	std::vector<std::string> textureDatas;

	void RemoveIfFlaged(std::vector<std::shared_ptr<std::atomic_bool>>& loadEndFlags);
};

