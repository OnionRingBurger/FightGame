#pragma once

#include "GameTexture.h"

class ShadowMap
{
public:
	ShadowMap();
	~ShadowMap();

	// !!!New!!!
	HRESULT Create(UINT width, UINT height);
	void Clear();

	// !!!New!!!
	bool IsValid() const;
	DepthStencil* GetDepthStencil();
	ID3D11ShaderResourceView* GetSRV() const;

private:
	DepthStencil m_depth;
	bool m_isValid;
};
