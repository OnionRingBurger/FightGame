#pragma once

#include "ShadowMap.h"
#include "Defines.h"

class DrawResources
{
public:
	DrawResources();
	~DrawResources();

	// !!!New!!!
	HRESULT Init();

	// !!!New!!!
	ShadowMap* GetShadowMap();
	const ShadowMap* GetShadowMap() const;

	// !!!New!!!
	void BeginShadowWrite();
	void EndShadowWrite();

private:
	ShadowMap m_shadowMap;
};
