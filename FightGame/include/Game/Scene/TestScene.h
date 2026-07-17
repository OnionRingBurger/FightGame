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

class TestScene : public IScene
{
public:
	TestScene();
	~TestScene();

	void AdvanceUpdate(float dt) override;
	void Update() override;
	void Draw() override;
	void PostFrameProcess() override;

private:

	std::vector<std::string> textureDatas;
};