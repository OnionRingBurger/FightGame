#pragma once
#include "IScene.h"
#include "World.h"

#include <memory>

class CreateGameScene : public IScene
{
public:
	CreateGameScene(IModelCacheAcquisition& modelCache,
		IUICacheAcquisition& uiCache);
	void AdvanceUpdate(float dt) override;
	void Update() override;
	void Draw() override;
	void PostFrameProcess() override;

private:
	std::unique_ptr<World> world;
};

