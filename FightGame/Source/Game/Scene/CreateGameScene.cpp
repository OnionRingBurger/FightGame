#include "CreateGameScene.h"
#include "CreateGameWorld.h"

CreateGameScene::CreateGameScene(
	IModelCacheAcquisition& modelCache,
	IUICacheAcquisition& uiCache,
	Input& a_input,
	ComponentsSerialize& a_serialize)
{


	world = std::make_unique<CreateGameWorld>(
		modelCache,
		uiCache,
		[this](int a_id) {},
		a_input,
		a_serialize
		);



	world->InitWorld();
}

void CreateGameScene::AdvanceUpdate(float dt)
{
	world->ContextUpdate(dt);
}

void CreateGameScene::Update()
{
	world->UpdateWorld();
}

void CreateGameScene::Draw()
{
	world->DrawWorld();
}

void CreateGameScene::PostFrameProcess()
{

}
