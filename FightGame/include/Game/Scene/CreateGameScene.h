#pragma once
#include "IScene.h"
#include "World.h"
#include "IEffectCacheAcquisition.h"

#include <memory>

class CreateGameScene : public IScene
{
public:
	CreateGameScene(
		IModelCacheAcquisition& modelCache,
		IUICacheAcquisition& uiCache,
		IEffectCacheAcquisition& a_effectCache,
		Input& a_input,
		ComponentsSerialize& a_serialize);
	void AdvanceUpdate(float dt) override;
	void Update() override;
	void Draw() override;
	void PostFrameProcess() override;

private:
	std::unique_ptr<World> world;
};

