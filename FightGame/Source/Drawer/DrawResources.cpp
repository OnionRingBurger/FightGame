#include "DrawResources.h"
#include "DirectX.h"

DrawResources::DrawResources()
{
}

DrawResources::~DrawResources()
{
}

HRESULT DrawResources::Init()
{
	// シャドウマップを作成
	return m_shadowMap.Create(static_cast<UINT>(kShadowMapSize), static_cast<UINT>(kShadowMapSize));
}

ShadowMap* DrawResources::GetShadowMap()
{
	// シャドウマップを取得
	if (!m_shadowMap.IsValid())
	{
		return nullptr;
	}
	return &m_shadowMap;
}

const ShadowMap* DrawResources::GetShadowMap() const
{
	if (!m_shadowMap.IsValid())
	{
		return nullptr;
	}
	return &m_shadowMap;
}

void DrawResources::BeginShadowWrite()
{
	// !!!New!!!
	if (!m_shadowMap.IsValid())
	{
		return;
	}
	m_shadowMap.Clear();
	SetDepthStencilTarget(m_shadowMap.GetDepthStencil());
}

void DrawResources::EndShadowWrite()
{
	// !!!New!!!
	RestoreDefaultRenderTargets();
}
