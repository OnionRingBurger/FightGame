#ifndef __SHADER_LIST_H__
#define __SHADER_LIST_H__

#include "Shader.h"
#include "Model.h"

class ShaderList
{
public:
	enum VSKind
	{
		VS_WORLD, // SetWVP
		VS_ANIME, // SetWVP,SetBones
		VS_KIND_MAX
	};
	enum PSKind
	{
		PS_LAMBERT, //SetMaterial, SetLight
		PS_SPECULAR, //SetMaterial, SetLight, SetCameraPos
		PS_TOON, // SetMaterial, SetLight
		PS_FOG, // SetMaterial, SetLight,SetFog
		PS_GHOST, // SetMaterial, SetLight, SetArea, SetGhostFade
		PS_ALPHA, // SetMaterial, SetLight
		PS_SHADOW_RECEIVE, // SetMaterial, SetLight, SetCamera, SetShadow, SetSectorDecals
		PS_ALPHA_SHADOW, // AlphaBlend + SetShadow
		PS_KIND_MAX
	};

	struct GhostArea
	{
		DirectX::XMFLOAT3 pos;
		float length;

		GhostArea(float a_posX, float a_posY, float a_posZ, float a_length)
			: pos(a_posX, a_posY, a_posZ)
			, length(a_length)
		{
		}
	};

	using GhostAreas = std::vector<GhostArea>;

	// !!!New!!!
	struct SectorDecalGPU
	{
		DirectX::XMFLOAT4 posYaw;
		DirectX::XMFLOAT4 sector;
		DirectX::XMFLOAT4 heightFade;
		DirectX::XMFLOAT4 color;

		SectorDecalGPU()
			: posYaw(0.0f, 0.0f, 0.0f, 0.0f)
			, sector(0.0f, 0.0f, 0.0f, 0.0f)
			, heightFade(0.0f, 0.0f, 0.0f, 0.0f)
			, color(1.0f, 1.0f, 1.0f, 1.0f)
		{
		}

		SectorDecalGPU(
			DirectX::XMFLOAT3 a_pos,
			float a_yaw,
			float a_minLength,
			float a_maxLength,
			float a_angle,
			float a_height,
			float a_lowness,
			float a_remaining,
			float a_maxDraw,
			DirectX::XMFLOAT4 a_color,
			float a_textureIndex
			)
			: posYaw(a_pos.x, a_pos.y, a_pos.z, a_yaw)
			, sector(a_minLength, a_maxLength, a_angle, a_textureIndex)
			, heightFade(a_height, a_lowness, a_remaining, a_maxDraw)
			, color(a_color)
		{
		}
	};

public:
	ShaderList();
	~ShaderList();

	static void Init();
	static void Uninit();

	static VertexShader* GetVS(VSKind vs);
	static PixelShader* GetPS(PSKind ps);

	static void SetWVP(DirectX::XMFLOAT4X4* wvp);
	static void SetBones(DirectX::XMFLOAT4X4* bones200);
	static void SetMaterial(const Model::Material& material);
	static void SetLight(DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 dir);
	static void SetCamera(const DirectX::XMFLOAT3 pos, const  DirectX::XMFLOAT3 forward);
	static void SetFog(DirectX::XMFLOAT4 color, float start, float range);
	static void SetArea(GhostAreas area);
	static void SetGhostFade(float ghostFadeRange);
	// !!!New!!!
	static void SetShadow(DirectX::XMFLOAT4X4* lightView, DirectX::XMFLOAT4X4* lightProj, float bias, float texelSize);
	// !!!New!!!
	static void SetSectorDecals(const SectorDecalGPU* decals, int count, Texture* const* textures, int textureCount);

private:
	static void MakeWorldVS();
	static void MakeAnimeVS();
	static void MakeLambertPS();
	static void MakeSpecularPS();
	static void MakeToonPS();
	static void MakeFogPS();
	static void MakeGhostPS();
	static void MakeAlphaBlend();
	static void MakeShadowReceivePS();
	static void MakeAlphaShadowPS();
	// !!!New!!!
	static void ReleaseSectorDecalArray();
	// !!!New!!!
	static bool EnsureSectorDecalArray(UINT width, UINT height, DXGI_FORMAT format);

private:
	static VertexShader* m_pVS[VS_KIND_MAX];
	static PixelShader* m_pPS[PS_KIND_MAX];
	// !!!New!!!
	static ID3D11Texture2D* m_pSectorDecalArrayTex;
	// !!!New!!!
	static ID3D11ShaderResourceView* m_pSectorDecalArraySRV;
	// !!!New!!!
	static UINT m_sectorDecalArrayWidth;
	// !!!New!!!
	static UINT m_sectorDecalArrayHeight;
	// !!!New!!!
	static DXGI_FORMAT m_sectorDecalArrayFormat;
};

#endif // __SHADER_LIST_H__
