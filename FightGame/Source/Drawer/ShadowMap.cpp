#include "ShadowMap.h"

ShadowMap::ShadowMap()
	: m_isValid(false)
{
}

ShadowMap::~ShadowMap()
{
}

HRESULT ShadowMap::Create(UINT width, UINT height)
{
	// シャドウマップを作成
	HRESULT hr = m_depth.Create(width, height, false);
	m_isValid = SUCCEEDED(hr);
	return hr;
}

void ShadowMap::Clear()
{
	// リセット
	if (!m_isValid)
	{
		return;
	}
	m_depth.Clear();
}

bool ShadowMap::IsValid() const
{
	return m_isValid;
}

DepthStencil* ShadowMap::GetDepthStencil()
{
	if (!m_isValid)
	{
		return nullptr;
	}
	return &m_depth;
}

ID3D11ShaderResourceView* ShadowMap::GetSRV() const
{
	if (!m_isValid)
	{
		return nullptr;
	}
	return m_depth.GetResource();
}
