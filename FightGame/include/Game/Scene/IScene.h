#pragma once
class IScene
{
public:
	IScene() = default;
	virtual ~IScene();

	virtual void AdvanceUpdate(float dt) = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void PostFrameProcess() = 0;
};

