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
		LoadJobQueues& a_jobQueues,
		LoadManifestPaths a_manifestPaths,
		IModelCacheAcquisition& a_modelCache,
		IUICacheAcquisition& a_uiCache,
		IEffectCacheAcquisition& effectCache,
		Input& a_input,
		ComponentsSerialize& a_serialize,
		PrefabManager& a_prefabManager);

	~LoadScene() = default;
	void AdvanceUpdate(float dt) override;
	void Update() override;
	void Draw() override;
	void PostFrameProcess() override;

private:
	bool inited;
	// !!!New!!!
	bool presentationReady;
	// !!!New!!!
	bool manifestRegistered;
	std::vector<std::shared_ptr<std::atomic_bool>> useDataLoadEndFlags;
	std::function<void(std::string)> sceneChange;
	std::vector<std::shared_ptr<std::atomic_bool>> loadEndFlags;
	std::unique_ptr<World> world;
	int maxLoadCount;
	LoadJobQueues& jobQueues;
	// !!!New!!!
	IUICacheAcquisition& uiCache;
	// !!!New!!!
	LoadManifestPaths manifestPaths;
	LoadDatas loadDatas;

	void RemoveIfFlaged(std::vector<std::shared_ptr<std::atomic_bool>>& loadEndFlags);
};
