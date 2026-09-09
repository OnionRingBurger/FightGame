#pragma once


#include <d3d11.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")


#include "Effekseer.h"
#include "EffekseerRendererDX11.h"


#define SAFE_DELETE(p)			do{if(p){delete p; p = nullptr;}}while(0)
#define SAFE_DELETE_ARRAY(p)	do{if(p){delete[] p; p = nullptr;}}while(0)
#define SAFE_RELEASE(p)			do{if(p){p->Release(); p = nullptr;}}while(0)

class RenderTarget;
class DepthStencil;

enum BlendMode
{
	BLEND_NONE,
	BLEND_ALPHA,
	BLEND_ADD,
	BLEND_ADDALPHA,
	BLEND_SUB,
	BLEND_SCREEN,
	BLEND_MAX
};

enum DepthMode
{
	DEPTH_TEST_TRUE,
	DEPTH_TEST_FALSE,
	DEPTH_TEST_COMPARISON_LESS
};

enum SamplerState
{
	SAMPLER_LINEAR,
	SAMPLER_POINT,
	SAMPLER_MAX
};

ID3D11Device* GetDevice();
ID3D11DeviceContext* GetContext();
IDXGISwapChain* GetSwapChain();
RenderTarget* GetDefaultRTV();
DepthStencil* GetDefaultDSV();
Effekseer::ManagerRef GetEffectManager();
EffekseerRenderer::RendererRef GetEffectRenderer();
IDWriteFactory* GetDWriteFactory();
ID2D1Factory* GetD2DFactory();
IWICImagingFactory* GetWICFactory();



HRESULT InitDirectX(HWND hWnd, UINT width, UINT height, bool fullscreen);
void UninitDirectX();
void BeginDrawDirectX();
void EndDrawDirectX();

void SetRenderTargets(UINT num, RenderTarget** ppViews, DepthStencil* pView);
void SetCullingMode(D3D11_CULL_MODE cull);
void SetDepthTest(DepthMode);
void SetBlendMode(BlendMode blend);
void SetSamplerState(SamplerState state);
