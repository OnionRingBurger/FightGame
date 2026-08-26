#pragma once
#include "Effekseer.h"
#include "EffekseerRendererDX11.h"
#include "MathAssist.h"

class EffectManager
{
	EffectManager();

	void Init();
	void UnInit();

	Effekseer::Handle PlayEffect(Effekseer::EffectRef effect, float3 pos);
	void UpdateEffect(Effekseer::Handle effect, float deltaTime);

private:

	Effekseer::ManagerRef managerRef;
	EffekseerRendererDX11::RendererRef rendererRef;
	Effekseer::Backend::GraphicsDeviceRef graphicsRef;


};