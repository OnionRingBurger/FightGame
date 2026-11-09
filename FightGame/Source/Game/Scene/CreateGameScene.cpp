#include "CreateGameScene.h"
#include "CreateGameWorld.h"

CreateGameScene::CreateGameScene(
	IModelCacheAcquisition& modelCache,
	IUICacheAcquisition& uiCache,
	IEffectCacheAcquisition& effectCache,
	Input& a_input,
	ComponentsSerialize& a_serialize,
	PrefabManager& a_prefabManager)
{


	world = std::make_unique<CreateGameWorld>(
		modelCache,
		uiCache,
		effectCache,
		[this](int a_id) {},
		a_input,
		a_serialize,
		a_prefabManager
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
