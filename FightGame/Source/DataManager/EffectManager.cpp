#include "EffectManager.h"
#include "DirectX.h"

EffectManager::EffectManager()
{

}

void EffectManager::Init()
{
	managerRef = Effekseer::Manager::Create(8000);

	graphicsRef = EffekseerRendererDX11::CreateGraphicsDevice(GetDevice(), GetContext());
	rendererRef = EffekseerRendererDX11::Renderer::Create(GetDevice(), GetContext(), 10000);
}

void EffectManager::UnInit()
{
}

Effekseer::Handle EffectManager::PlayEffect(Effekseer::EffectRef effect, float3 pos)
{
	Effekseer::Vector3D vector(pos.x, pos.y, pos.z);
	return managerRef->Play(effect, vector);
}

void EffectManager::UpdateEffect(Effekseer::Handle effect, float deltaTime)
{
	managerRef->UpdateHandle(effect, deltaTime);
}
