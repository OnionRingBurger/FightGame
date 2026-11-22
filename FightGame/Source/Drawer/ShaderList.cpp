#include "ShaderList.h"
#include "Defines.h"
#include "MathAssist.h"
#include "GameData.h"
#include "DirectX.h"
#include <algorithm>

VertexShader* ShaderList::m_pVS[VS_KIND_MAX];
PixelShader* ShaderList::m_pPS[PS_KIND_MAX];
// !!!New!!!
ID3D11Texture2D* ShaderList::m_pSectorDecalArrayTex = nullptr;
// !!!New!!!
ID3D11ShaderResourceView* ShaderList::m_pSectorDecalArraySRV = nullptr;
// !!!New!!!
UINT ShaderList::m_sectorDecalArrayWidth = 0;
// !!!New!!!
UINT ShaderList::m_sectorDecalArrayHeight = 0;
// !!!New!!!
DXGI_FORMAT ShaderList::m_sectorDecalArrayFormat = DXGI_FORMAT_UNKNOWN;


ShaderList::ShaderList()
{
}
ShaderList::~ShaderList()
{
}

void ShaderList::Init()
{
	MakeWorldVS();
	MakeAnimeVS();
	MakeLambertPS();
	MakeSpecularPS();
	MakeToonPS();
	MakeFogPS();
	MakeGhostPS();
	MakeAlphaBlend();
	// !!!New!!!
	MakeShadowReceivePS();
	// !!!New!!!
	MakeAlphaShadowPS();

	DirectX::XMFLOAT4X4 mat[250];
	for (int i = 0; i < 250; ++i)
	{
		DirectX::XMStoreFloat4x4(&mat[i], DirectX::XMMatrixIdentity());
	}
	SetWVP(mat);
	SetBones(mat);

	Model::Material material = {
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		nullptr
	};
	SetMaterial(material);
	SetLight(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f));
	SetCamera(DirectX::XMFLOAT3(0.0f, 1.0f, -2.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f));
	SetFog(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 50.0f, 50.0f);
	SetArea(std::vector<GhostArea>());
	SetGhostFade(20.0f);
	// !!!New!!!
	DirectX::XMFLOAT4X4 identity;
	DirectX::XMStoreFloat4x4(&identity, DirectX::XMMatrixIdentity());
	SetShadow(&identity, &identity, 0.001f, 1.0f / static_cast<float>(kShadowMapSize));
	// !!!New!!!
	SetSectorDecals(nullptr, 0, nullptr, 0);
}

void ShaderList::Uninit()
{
	// !!!New!!!
	ReleaseSectorDecalArray();
	for (int i = 0; i < VS_KIND_MAX; ++i)
	{
		if (m_pVS[i])
		{
			delete m_pVS[i];
			m_pVS[i] = nullptr;
		}
	}
	for (int i = 0; i < PS_KIND_MAX; ++i)
	{
		if (m_pPS[i])
		{
			delete m_pPS[i];
			m_pPS[i] = nullptr;
		}
	}
}

VertexShader* ShaderList::GetVS(VSKind vs)
{
	return m_pVS[vs];
}
PixelShader* ShaderList::GetPS(PSKind ps)
{
	return m_pPS[ps];
}

void ShaderList::SetWVP(DirectX::XMFLOAT4X4* wvp)
{
	for (int i = 0; i < VS_KIND_MAX; ++i)
	{
		m_pVS[i]->WriteBuffer(0, wvp);
	}
}
void ShaderList::SetBones(DirectX::XMFLOAT4X4* bones200)
{
	m_pVS[VS_ANIME]->WriteBuffer(1, bones200);
}
void ShaderList::SetMaterial(const Model::Material& material)
{
	DirectX::XMFLOAT4 param[3] = {
		material.diffuse,
		material.ambient,
		material.specular
	};
	param[1].w = material.pTexture ? 1.0f : 0.0f;
	for (int i = 0; i < PS_KIND_MAX; ++i)
	{
		m_pPS[i]->SetTexture(0, material.pTexture);
		m_pPS[i]->WriteBuffer(0, param);
	}
}
void ShaderList::SetLight(DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 dir)
{
	DirectX::XMFLOAT4 param[2] = {
		color
	};
	DirectX::XMStoreFloat4(&param[1], DirectX::XMVector3Normalize(
		DirectX::XMVectorSet(dir.x, dir.y, dir.z, 0.0f)
	));
	for (int i = 0; i < PS_KIND_MAX; ++i)
	{
		m_pPS[i]->WriteBuffer(1, param);
	}
}
void ShaderList::SetCamera(const DirectX::XMFLOAT3 pos, const  DirectX::XMFLOAT3 forward)
{
	DirectX::XMFLOAT4 param[] = {
		{pos.x, pos.y, pos.z, 0.0f},
		{forward.x, forward.y, forward.z, 0.0f}

	};
	m_pPS[PS_SPECULAR]->WriteBuffer(2, param);
	m_pPS[PS_TOON]->WriteBuffer(2, param);
	m_pPS[PS_GHOST]->WriteBuffer(2, param);
	// !!!New!!!
	m_pPS[PS_SHADOW_RECEIVE]->WriteBuffer(2, param);
}
void ShaderList::SetFog(DirectX::XMFLOAT4 color, float start, float range)
{
	DirectX::XMFLOAT4 param[] = {
		color,
		{start, range, 0.0f, 0.0f}
	};
	m_pPS[PS_FOG]->WriteBuffer(3, param);
}

void ShaderList::SetArea(GhostAreas areas)
{
	DirectX::XMFLOAT4 param[kMaxArea + 1];
	int areaCount = std::min(100, (int)areas.size());
	for (int i = 0; i < areas.size(); i++)
	{
		param[i] = {
			areas[i].pos.x, areas[i].pos.y, areas[i].pos.z, areas[i].length 
		};
	}
	param[kMaxArea] = {
		(float)areaCount, 0.0f, 0.0f, 0.0f
	};
	
	m_pPS[PS_GHOST]->WriteBuffer(3, &param);
}

void ShaderList::SetGhostFade(float ghostFadeRange)
{
	DirectX::XMFLOAT4 param = { ghostFadeRange, 0.0f, 0.0f, 0.0f };
	m_pPS[PS_GHOST]->WriteBuffer(4, &param);
}

void ShaderList::SetShadow(DirectX::XMFLOAT4X4* lightView, DirectX::XMFLOAT4X4* lightProj, float bias, float texelSize)
{
	// !!!New!!!
	struct ShadowParam
	{
		DirectX::XMFLOAT4X4 lightView;
		DirectX::XMFLOAT4X4 lightProj;
		DirectX::XMFLOAT4 shadowParam;
	};
	ShadowParam param;
	param.lightView = *lightView;
	param.lightProj = *lightProj;
	param.shadowParam = DirectX::XMFLOAT4(bias, texelSize, 0.0f, 0.0f);
	m_pPS[PS_SHADOW_RECEIVE]->WriteBuffer(3, &param);
	// !!!New!!!
	m_pPS[PS_ALPHA_SHADOW]->WriteBuffer(3, &param);
}

// デカールに扇形の範囲を設定
void ShaderList::ReleaseSectorDecalArray()
{
	SAFE_RELEASE(m_pSectorDecalArraySRV);
	SAFE_RELEASE(m_pSectorDecalArrayTex);
	m_sectorDecalArrayWidth = 0;
	m_sectorDecalArrayHeight = 0;
	m_sectorDecalArrayFormat = DXGI_FORMAT_UNKNOWN;
}

bool ShaderList::EnsureSectorDecalArray(UINT width, UINT height, DXGI_FORMAT format)
{
	if (m_pSectorDecalArrayTex != nullptr &&
		m_sectorDecalArrayWidth == width &&
		m_sectorDecalArrayHeight == height &&
		m_sectorDecalArrayFormat == format)
	{
		return true;
	}

	ReleaseSectorDecalArray();

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = static_cast<UINT>(kMaxSectorDecals);
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = GetDevice()->CreateTexture2D(&desc, nullptr, &m_pSectorDecalArrayTex);
	if (FAILED(hr))
	{
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = static_cast<UINT>(kMaxSectorDecals);

	hr = GetDevice()->CreateShaderResourceView(m_pSectorDecalArrayTex, &srvDesc, &m_pSectorDecalArraySRV);
	if (FAILED(hr))
	{
		ReleaseSectorDecalArray();
		return false;
	}

	m_sectorDecalArrayWidth = width;
	m_sectorDecalArrayHeight = height;
	m_sectorDecalArrayFormat = format;
	return true;
}

// !!!New!!!
static void BindSectorDecalSRV(ID3D11ShaderResourceView* srv)
{
	ShaderList::GetPS(ShaderList::PS_SHADOW_RECEIVE)->SetTextureSRV(2, srv);
	ShaderList::GetPS(ShaderList::PS_ALPHA_SHADOW)->SetTextureSRV(3, srv);
}

void ShaderList::SetSectorDecals(const SectorDecalGPU* decals, int count, Texture* const* textures, int textureCount)
{
	struct SectorDecalBuffer
	{
		DirectX::XMFLOAT4 decalInfo;
		SectorDecalGPU decals[kMaxSectorDecals];
	};
	SectorDecalBuffer param = {};
	const int useCount = std::min(kMaxSectorDecals, std::max(0, count));
	const int useTexCount = std::min(kMaxSectorDecals, std::max(0, textureCount));
	param.decalInfo = DirectX::XMFLOAT4(static_cast<float>(useCount), 0.0f, 0.0f, 0.0f);
	if (decals != nullptr)
	{
		for (int i = 0; i < useCount; ++i)
		{
			param.decals[i] = decals[i];
		}
	}
	m_pPS[PS_SHADOW_RECEIVE]->WriteBuffer(4, &param);
	// !!!New!!!
	m_pPS[PS_ALPHA_SHADOW]->WriteBuffer(4, &param);

	if (textures == nullptr || useTexCount <= 0)
	{
		BindSectorDecalSRV(nullptr);
		return;
	}

	Texture* firstTex = nullptr;
	for (int i = 0; i < useTexCount; ++i)
	{
		if (textures[i] != nullptr && textures[i]->GetResource() != nullptr)
		{
			firstTex = textures[i];
			break;
		}
	}
	if (firstTex == nullptr)
	{
		BindSectorDecalSRV(nullptr);
		return;
	}

	ID3D11Resource* firstRes = nullptr;
	firstTex->GetResource()->GetResource(&firstRes);
	ID3D11Texture2D* firstTex2D = nullptr;
	if (firstRes == nullptr || FAILED(firstRes->QueryInterface(IID_PPV_ARGS(&firstTex2D))))
	{
		SAFE_RELEASE(firstRes);
		BindSectorDecalSRV(nullptr);
		return;
	}

	D3D11_TEXTURE2D_DESC srcDesc = {};
	firstTex2D->GetDesc(&srcDesc);
	SAFE_RELEASE(firstTex2D);
	SAFE_RELEASE(firstRes);

	if (!EnsureSectorDecalArray(srcDesc.Width, srcDesc.Height, srcDesc.Format))
	{
		BindSectorDecalSRV(nullptr);
		return;
	}

	for (int i = 0; i < useTexCount; ++i)
	{
		if (textures[i] == nullptr || textures[i]->GetResource() == nullptr)
		{
			continue;
		}

		ID3D11Resource* srcRes = nullptr;
		textures[i]->GetResource()->GetResource(&srcRes);
		if (srcRes == nullptr)
		{
			continue;
		}

		ID3D11Texture2D* srcTex2D = nullptr;
		if (SUCCEEDED(srcRes->QueryInterface(IID_PPV_ARGS(&srcTex2D))))
		{
			D3D11_TEXTURE2D_DESC sliceDesc = {};
			srcTex2D->GetDesc(&sliceDesc);
			if (sliceDesc.Width == m_sectorDecalArrayWidth &&
				sliceDesc.Height == m_sectorDecalArrayHeight &&
				sliceDesc.Format == m_sectorDecalArrayFormat)
			{
				const UINT dstSubresource = D3D11CalcSubresource(0, static_cast<UINT>(i), 1);
				GetContext()->CopySubresourceRegion(
					m_pSectorDecalArrayTex,
					dstSubresource,
					0, 0, 0,
					srcRes,
					0,
					nullptr);
			}
			SAFE_RELEASE(srcTex2D);
		}
		SAFE_RELEASE(srcRes);
	}

	BindSectorDecalSRV(m_pSectorDecalArraySRV);
}

void ShaderList::MakeWorldVS()
{
	const char* code = R"EOT(
struct VS_IN {
	float3 pos : POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};
struct VS_OUT {
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
	float4 wPos : POSITION0;
};
cbuffer WVP : register(b0) {
	float4x4 world;
	float4x4 view;
	float4x4 proj;
};
VS_OUT main(VS_IN vin) {
	VS_OUT vout;
	vout.pos = float4(vin.pos, 1.0f);
	vout.pos = mul(vout.pos, world);
	vout.wPos = vout.pos;
	vout.pos = mul(vout.pos, view);
	vout.pos = mul(vout.pos, proj);
	vout.normal = mul(vin.normal, (float3x3)world);
	vout.uv = vin.uv;
	vout.color = vin.color;
	return vout;
})EOT";
	m_pVS[VS_WORLD] = new VertexShader();
	m_pVS[VS_WORLD]->Compile(code);
}
void ShaderList::MakeAnimeVS()
{
	const char* code = R"EOT(
struct VS_IN {
	float3 pos : POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
	float4 weight : WEIGHT0;
	uint4 index : INDEX0;
};
struct VS_OUT {
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
	float4 wPos : POSITION0;
};
cbuffer WVP : register(b0) {
	float4x4 world;
	float4x4 view;
	float4x4 proj;
};
cbuffer Bone : register(b1) {
	float4x4 bone[200];
};
VS_OUT main(VS_IN vin) {
	VS_OUT vout;
	float4x4 anime;
	anime  = bone[vin.index.x] * vin.weight.x;
	anime += bone[vin.index.y] * vin.weight.y;
	anime += bone[vin.index.z] * vin.weight.z;
	anime += bone[vin.index.w] * vin.weight.w;
	vout.pos = float4(vin.pos, 1.0f);
	vout.pos = mul(vout.pos, anime);
	vout.pos = mul(vout.pos, world);
	vout.wPos = vout.pos;
	vout.pos = mul(vout.pos, view);
	vout.pos = mul(vout.pos, proj);
	vout.normal = vin.normal;
	vout.normal = mul(vout.normal, (float3x3)anime);
	vout.normal = mul(vout.normal, (float3x3)world);
	vout.uv = vin.uv;
	vout.color = vin.color;
	return vout;
})EOT";
	m_pVS[VS_ANIME] = new VertexShader();
	m_pVS[VS_ANIME]->Compile(code);
}


void ShaderList::MakeLambertPS()
{
	const char* code = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};
cbuffer Material : register(b0)
{
	float4 objDiffuse;
	float4 objAmbient;
	float4 objSpecular;
};
cbuffer Light : register(b1)
{
	float4 lightDiffuse;
	float4 lightDir;
};
Texture2D tex : register(t0);
SamplerState samp : register(s0);
float4 main(PS_IN pin) : SV_TARGET
{
	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	if(objAmbient.a >= 1.0f)
		color = tex.Sample(samp, pin.uv);
	float3 N = normalize(pin.normal);
	float3 L = normalize(-lightDir);
	float dotNL = saturate((dot(N, L) + 0.5f) / 1.5f);
	float3 diffuse = objDiffuse.rgb * lightDiffuse.rgb;
	float3 ambient = objAmbient.rgb * lightDiffuse.rgb;
	float3 specular = objSpecular.rgb * lightDiffuse.rgb;
	// 本来のLambert拡散反射（思った表現が出来なかったので採用せず
	// color.rgb *= saturate(diffuse * dotNL + ambient);
	// 環境光で拡散反射部分の色が変わらないようにlerp(環境光,diffuse,dotNL)で計算
	// 環境光が弱ければ黒(乗算)、強ければ白(加算)となるように、各計算を線形で補間
	diffuse *= color.rgb;
	color.rgb = saturate(lerp(
		lerp(diffuse * ambient, diffuse + ambient, pow(ambient, 4.0f)),
		diffuse, dotNL));
	// 本来なら必要ない鏡面反射、Lambert向けに若干だけ適用
	color.rgb += specular * pow(saturate(dotNL), max(0.01f, objSpecular.a) * 0.5f) * 0.5f;
	return color;
})EOT";
	m_pPS[PS_LAMBERT] = new PixelShader();
	m_pPS[PS_LAMBERT]->Compile(code);
}
void ShaderList::MakeSpecularPS()
{
	const char* code = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
	float4 wPos : POSITION0;
};
cbuffer Material : register(b0)
{
	float4 objDiffuse;
	float4 objAmbient;
	float4 objSpecular;
};
cbuffer Light : register(b1)
{
	float4 lightDiffuse;
	float4 lightDir;
};
cbuffer Camera : register(b2)
{
	float4 cameraPos;
};
Texture2D tex : register(t0);
SamplerState samp : register(s0);
float4 main(PS_IN pin) : SV_TARGET
{
	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	if(objAmbient.a >= 1.0f)
		color = tex.Sample(samp, pin.uv);
	float3 N = normalize(pin.normal);
	float3 L = normalize(-lightDir);
	float3 V = normalize(cameraPos.xyz - pin.wPos.xyz);
	float3 R = reflect(-V, N);
	float dotNL = saturate((dot(N, L) + 0.5f) / 1.5f);
	float dotRL = saturate(dot(R, L));
	float3 diffuse = objDiffuse.rgb * lightDiffuse.rgb;
	float3 ambient = objAmbient.rgb * lightDiffuse.rgb;
	float3 specular = objSpecular.rgb * lightDiffuse.rgb;
	// Lambertの計算を参考
	color.rgb *= saturate(lerp(
		lerp(diffuse * ambient, diffuse + ambient, pow(ambient, 4.0f)),
		diffuse, dotNL));
	color.rgb += specular * saturate(pow(dotRL, max(0.01f, objSpecular.a)));
	return color;
})EOT";
	m_pPS[PS_SPECULAR] = new PixelShader();
	m_pPS[PS_SPECULAR]->Compile(code);
}
void ShaderList::MakeToonPS()
{
	const char* code = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};
cbuffer Material : register(b0)
{
	float4 objDiffuse;
	float4 objAmbient;
	float4 objSpecular;
};
cbuffer Light : register(b1)
{
	float4 lightDiffuse;
	float4 lightDir;
};
Texture2D tex : register(t0);
SamplerState samp : register(s0);
float4 main(PS_IN pin) : SV_TARGET
{
	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	if(objAmbient.a >= 1.0f)
		color = tex.Sample(samp, pin.uv);
	float3 N = normalize(pin.normal);
	float3 L = normalize(-lightDir);
	float dotNL = dot(N, L); // マイナス込で計算
	float3 diffuse = objDiffuse.rgb * lightDiffuse.rgb;
	float3 ambient = objAmbient.rgb * lightDiffuse.rgb;
	float3 specular = objSpecular.rgb * lightDiffuse.rgb;
	float toonNL = saturate((dot(N, L) + 0.5f) / 1.5f * 100.0f); // 陰の境目を柔らかく
	// Lambertの計算を参考
	color.rgb *= saturate(lerp(
		lerp(diffuse * ambient, diffuse + ambient, pow(ambient, 4.0f)),
		diffuse, toonNL));
	if(objSpecular.a >= 1.0f)
		color.rgb += specular * saturate(pow(dotNL, max(0.01f, objSpecular.a)) * 100.0f);
	return color;
})EOT";
	m_pPS[PS_TOON] = new PixelShader();
	m_pPS[PS_TOON]->Compile(code);
}
void ShaderList::MakeFogPS()
{
	const char* code = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
	float4 wPos : POSITION0;
};
cbuffer Material : register(b0)
{
	float4 objDiffuse;
	float4 objAmbient;
	float4 objSpecular;
};
cbuffer Light : register(b1)
{
	float4 lightDiffuse;
	float4 lightDir;
};
cbuffer Camera : register(b2)
{
	float4 cameraPos;
};
cbuffer Param : register(b3)
{
	float4 fogColor;
	float fogStart;
	float fogRange;
	float2 dummy;
};
Texture2D tex : register(t0);
SamplerState samp : register(s0);
float4 main(PS_IN pin) : SV_TARGET
{
	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	if(objAmbient.a >= 1.0f)
		color = tex.Sample(samp, pin.uv);
	float3 N = normalize(pin.normal);
	float3 L = normalize(-lightDir);
	float dotNL = saturate((dot(N, L) + 0.5f) / 1.5f);
	float3 diffuse = objDiffuse.rgb * lightDiffuse.rgb;
	float3 ambient = objAmbient.rgb * lightDiffuse.rgb;
	color.rgb *= saturate(diffuse * dotNL + ambient);
	float vLen = length(pin.wPos.xyz - cameraPos.xyz);
	color.rgb = lerp(color.rgb, fogColor.rgb, saturate((vLen - fogStart) / fogRange));
	return color;
})EOT";
	m_pPS[PS_FOG] = new PixelShader();
	m_pPS[PS_FOG]->Compile(code);
}

void ShaderList::MakeGhostPS()
{
	const char* code = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
	float4 wPos : POSITION0;
};
cbuffer Material : register(b0)
{
	float4 objDiffuse;
	float4 objAmbient;
	float4 objSpecular;
};
cbuffer Light : register(b1)
{
	float4 lightDiffuse;
	float4 lightDir;
};
cbuffer Camera : register(b2)
{
	float4 cameraPos;
	float3 cameraForward;
	float dummyCameraForward;
};
cbuffer Area : register(b3)
{
	float4 posAndLengths[100];
	float4 areaData;
};
cbuffer GhostFade : register(b4)
{
	float4 ghostFadeData;
};
Texture2D tex : register(t0);
SamplerState samp : register(s0);
float4 main(PS_IN pin) : SV_TARGET
{
	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	if(objAmbient.a >= 1.0f)
		color = tex.Sample(samp, pin.uv);
	float3 N = normalize(pin.normal);
	float3 L = normalize(-lightDir);
	float3 V = normalize(cameraPos.xyz - pin.wPos.xyz);
	float3 R = reflect(-V, N);
	float dotNL = saturate((dot(N, L) + 0.5f) / 1.5f);
	float dotRL = saturate(dot(R, L));
	float3 diffuse = objDiffuse.rgb * lightDiffuse.rgb;
	float3 ambient = objAmbient.rgb * lightDiffuse.rgb;
	float3 specular = objSpecular.rgb * lightDiffuse.rgb;
	// Lambertの計算を参考
	color.rgb *= saturate(lerp(
		lerp(diffuse * ambient, diffuse + ambient, pow(ambient, 4.0f)),
		diffuse, dotNL));
	color.rgb += specular * saturate(pow(dotRL, max(0.01f, objSpecular.a)));

	// 範囲に応じて透明度を変更

	float areaValue = 0.0f;
	for(int i = 0; i < areaData.x; i++)
	{
		float areaDis = distance(pin.wPos.xyz, posAndLengths[i].xyz);
		float currentAlpha = (posAndLengths[i].w - areaDis) / posAndLengths[i].w;
		areaValue = max(areaValue, currentAlpha);	
	}

	
	float3 fogRGB = float3(0.7f, 0.7f, 0.7f);
	float3 finalRGB = lerp(color.rgb, fogRGB, areaValue);
	color.rgb *= finalRGB;
	float finalAlpha = lerp(0.05f, 0.75f, areaValue);
	color.a = areaValue;
	
	// 距離に応じてシルエットを表示
	float camDis = distance(pin.wPos.xyz, cameraPos.xyz);
	float ghostFadeRange = max(ghostFadeData.x, 0.0001f);
	float fadeByDistance = exp(-camDis / ghostFadeRange);

	float3 cameraVector = normalize(cameraPos.xyz - pin.pos.xyz);	
	
	float lineDot = saturate(abs(dot(cameraVector, pin.normal)));
	float lineValue = pow(1.0f - lineDot, 2);
	//color.rgb += lineValue;
	// color.a = saturate(color.a + lineValue * fadeByDistance) * 0.7;
	// color.a = saturate(color.a + lineValue) * 0.7;
	// color += 0.003f;


	return color;
})EOT";
	m_pPS[PS_GHOST] = new PixelShader();
	m_pPS[PS_GHOST]->Compile(code);
}

void ShaderList::MakeAlphaBlend()
{
	const char* code = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
	float4 wPos : POSITION0;
};
cbuffer Material : register(b0)
{
	float4 objDiffuse;
	float4 objAmbient;
	float4 objSpecular;
};
cbuffer Light : register(b1)
{
	float4 lightDiffuse;
	float4 lightDir;
};
cbuffer Camera : register(b2)
{
	float4 cameraPos;
	float3 cameraForward;
	float dummyCameraForward;
};
Texture2D alphaTex1 : register(t0);
Texture2D alphaTex2 : register(t1);
SamplerState samp : register(s0);
float4 main(PS_IN pin) : SV_TARGET
{
	float4 alphaColor1 = alphaTex1.Sample(samp, pin.uv);
	float4 alphaColor2 = alphaTex2.Sample(samp, pin.uv);

	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	color.xyz = (alphaColor1.xyz + alphaColor2.xyz) / 1.5f;	
	
	return color;
})EOT";
	m_pPS[PS_ALPHA] = new PixelShader();
	m_pPS[PS_ALPHA]->Compile(code);
}

void ShaderList::MakeShadowReceivePS()
{
	// !!!New!!!
	const char* code = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
	float4 wPos : POSITION0;
};
cbuffer Material : register(b0)
{
	float4 objDiffuse;
	float4 objAmbient;
	float4 objSpecular;
};
cbuffer Light : register(b1)
{
	float4 lightDiffuse;
	float4 lightDir;
};
cbuffer Camera : register(b2)
{
	float4 cameraPos;
};
cbuffer Shadow : register(b3)
{
	float4x4 lightView;
	float4x4 lightProj;
	float4 shadowParam;
};
struct SectorDecalGPU
{
	float4 posYaw;
	float4 sector;
	float4 heightFade;
	float4 color;
};
cbuffer SectorDecals : register(b4)
{
	float4 decalInfo;
	SectorDecalGPU decals[8];
};
Texture2D tex : register(t0);
Texture2D shadowMap : register(t1);
Texture2DArray decalTex : register(t2);
SamplerState samp : register(s0);
SamplerComparisonState shadowSamp : register(s1);
float4 main(PS_IN pin) : SV_TARGET
{
	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	if(objAmbient.a >= 1.0f)
		color = tex.Sample(samp, pin.uv);
	float3 N = normalize(pin.normal);
	float3 L = normalize(-lightDir);
	float3 V = normalize(cameraPos.xyz - pin.wPos.xyz);
	float3 R = reflect(-V, N);
	float dotNL = saturate((dot(N, L) + 0.5f) / 1.5f);
	float dotRL = saturate(dot(R, L));
	float3 diffuse = objDiffuse.rgb * lightDiffuse.rgb;
	float3 ambient = objAmbient.rgb * lightDiffuse.rgb;
	float3 specular = objSpecular.rgb * lightDiffuse.rgb;
	color.rgb *= saturate(lerp(
		lerp(diffuse * ambient, diffuse + ambient, pow(ambient, 4.0f)),
		diffuse, dotNL));
	color.rgb += specular * saturate(pow(dotRL, max(0.01f, objSpecular.a)));

	float4 lightPos = float4(pin.wPos.xyz, 1.0f);
	lightPos = mul(lightPos, lightView);
	lightPos = mul(lightPos, lightProj);
	float3 proj = lightPos.xyz / lightPos.w;
	float2 shadowUV = float2(proj.x * 0.5f + 0.5f, proj.y * -0.5f + 0.5f);
	float bias = shadowParam.x;
	float texel = shadowParam.y;
	float shadow = 0.0f;
	[unroll]
	for (int y = -1; y <= 1; ++y)
	{
		[unroll]
		for (int x = -1; x <= 1; ++x)
		{
			float2 offset = float2(x, y) * texel;
			shadow += shadowMap.SampleCmpLevelZero(shadowSamp, shadowUV + offset, proj.z - bias);
		}
	}
	shadow /= 9.0f;
	color.rgb *= lerp(0.35f, 1.0f, shadow);

	int decalCount = (int)decalInfo.x;
	[loop]
	for (int i = 0; i < decalCount; ++i)
	{
		float3 center = decals[i].posYaw.xyz;
		float yawRad = decals[i].posYaw.w * 0.01745329251f;
		float2 forward = float2(sin(yawRad), cos(yawRad));
		float2 right = float2(cos(yawRad), -sin(yawRad));
		float minLength = decals[i].sector.x;
		float maxLength = decals[i].sector.y;
		float angle = decals[i].sector.z;
		float textureIndex = decals[i].sector.w;
		float maxHeight = decals[i].heightFade.x;
		float maxLowness = decals[i].heightFade.y;
		float remaining = decals[i].heightFade.z;
		float maxDraw = decals[i].heightFade.w;
		float2 deltaXZ = pin.wPos.xz - center.xz;
		float r = length(deltaXZ);
		if (r < minLength || r > maxLength)
		{
			continue;
		}
		float cosHalf = cos(angle * 0.5f * 0.01745329251f);
		float2 dir = deltaXZ / max(r, 1e-5f);
		float cosTheta = dot(forward, dir);
		if (cosTheta < cosHalf)
		{
			continue;
		}
		float dy = pin.wPos.y - center.y;
		if (dy > maxHeight || dy < -maxLowness)
		{
			continue;
		}
		float localX = dot(deltaXZ, right);
		float localZ = dot(deltaXZ, forward);
		float halfWidth = max(maxLength * sin(angle * 0.5f * 0.01745329251f), 1e-5f);
		float u = localX / halfWidth * 0.5f + 0.5f;
		float v = localZ / max(maxLength, 1e-5f);
		if (u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f)
		{
			continue;
		}
		float4 decalSample = decalTex.Sample(samp, float3(u, v, textureIndex));
		float fade = saturate(remaining / max(maxDraw, 1e-5f));
		float alpha = saturate(decalSample.a * decals[i].color.a * fade);
		// !!!New!!!
		float3 decalWhite = float3(1.0f, 1.0f, 1.0f);
		color.rgb = lerp(color.rgb, decalSample.rgb * decalWhite, alpha);
	}
	return color;
})EOT";
	m_pPS[PS_SHADOW_RECEIVE] = new PixelShader();
	m_pPS[PS_SHADOW_RECEIVE]->Compile(code);
}

void ShaderList::MakeAlphaShadowPS()
{
	const char* code = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
	float4 wPos : POSITION0;
};
cbuffer Material : register(b0)
{
	float4 objDiffuse;
	float4 objAmbient;
	float4 objSpecular;
};
cbuffer Light : register(b1)
{
	float4 lightDiffuse;
	float4 lightDir;
};
cbuffer Camera : register(b2)
{
	float4 cameraPos;
};
cbuffer Shadow : register(b3)
{
	float4x4 lightView;
	float4x4 lightProj;
	float4 shadowParam;
};
// !!!New!!!
struct SectorDecalGPU
{
	float4 posYaw;
	float4 sector;
	float4 heightFade;
	float4 color;
};
cbuffer SectorDecals : register(b4)
{
	float4 decalInfo;
	SectorDecalGPU decals[8];
};
Texture2D alphaTex1 : register(t0);
Texture2D alphaTex2 : register(t1);
Texture2D shadowMap : register(t2);
Texture2DArray decalTex : register(t3);
SamplerState samp : register(s0);
SamplerComparisonState shadowSamp : register(s1);
// !!!New!!!
float FrameHash(float2 p)
{
	return frac(sin(dot(p, float2(127.1f, 311.7f))) * 43758.5453f);
}
float FrameNoise(float2 p)
{
	float2 ip = floor(p);
	float2 fp = frac(p);
	float2 w = fp * fp * (3.0f - 2.0f * fp);
	float a = FrameHash(ip);
	float b = FrameHash(ip + float2(1.0f, 0.0f));
	float c = FrameHash(ip + float2(0.0f, 1.0f));
	float d = FrameHash(ip + float2(1.0f, 1.0f));
	return lerp(lerp(a, b, w.x), lerp(c, d, w.x), w.y);
}
float4 main(PS_IN pin) : SV_TARGET
{
	float4 alphaColor1 = alphaTex1.Sample(samp, pin.uv);
	float4 alphaColor2 = alphaTex2.Sample(samp, pin.uv);
	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	color.xyz = (alphaColor1.xyz + alphaColor2.xyz) / 1.5f;
	color.rgb += (objDiffuse.rgb + lightDiffuse.rgb + cameraPos.xyz) * 0.0f;

	float4 lightPos = float4(pin.wPos.xyz, 1.0f);
	lightPos = mul(lightPos, lightView);
	lightPos = mul(lightPos, lightProj);
	float3 proj = lightPos.xyz / lightPos.w;
	float2 shadowUV = float2(proj.x * 0.5f + 0.5f, proj.y * -0.5f + 0.5f);
	float bias = shadowParam.x;
	float texel = shadowParam.y;
	float shadow = 0.0f;
	[unroll]
	for (int y = -1; y <= 1; ++y)
	{
		[unroll]
		for (int x = -1; x <= 1; ++x)
		{
			float2 offset = float2(x, y) * texel;
			shadow += shadowMap.SampleCmpLevelZero(shadowSamp, shadowUV + offset, proj.z - bias);
		}
	}
	shadow /= 9.0f;
	color.rgb *= lerp(0.35f, 1.0f, shadow);

	// !!!New!!!
	int decalCount = (int)decalInfo.x;
	[loop]
	for (int i = 0; i < decalCount; ++i)
	{
		float3 center = decals[i].posYaw.xyz;
		float yawRad = decals[i].posYaw.w * 0.01745329251f;
		float2 forward = float2(sin(yawRad), cos(yawRad));
		float2 right = float2(cos(yawRad), -sin(yawRad));
		float minLength = decals[i].sector.x;
		float maxLength = decals[i].sector.y;
		float angle = decals[i].sector.z;
		float textureIndex = decals[i].sector.w;
		float maxHeight = decals[i].heightFade.x;
		float maxLowness = decals[i].heightFade.y;
		float remaining = decals[i].heightFade.z;
		float maxDraw = decals[i].heightFade.w;
		float2 deltaXZ = pin.wPos.xz - center.xz;
		float r = length(deltaXZ);
		float cosHalf = cos(angle * 0.5f * 0.01745329251f);
		float2 dir = deltaXZ / max(r, 1e-5f);
		float cosTheta = dot(forward, dir);
		float dy = pin.wPos.y - center.y;
		if (dy > maxHeight || dy < -maxLowness)
		{
			continue;
		}
		// !!!New!!!
		bool isFullCircle = angle >= 360.0f;
		float localX = dot(deltaXZ, right);
		float localZ = dot(deltaXZ, forward);
		float halfWidth = max(maxLength * sin(angle * 0.5f * 0.01745329251f), 1e-5f);
		float u = isFullCircle ? localX / maxLength * 0.5f + 0.5f : localX / halfWidth * 0.5f + 0.5f;
		float v = isFullCircle ? localZ / maxLength * 0.5f + 0.5f : localZ / max(maxLength, 1e-5f);
		if (u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f)
		{
			continue;
		}
		// !!!New!!!
		bool inFan = r >= minLength && r <= maxLength && cosTheta >= cosHalf;
		if (!inFan)
		{
			continue;
		}
		// !!!New!!!
		float rangeLength = max(maxLength - minLength, 1e-5f);
		float outerRatio = (maxLength - r) / rangeLength;
		float innerRatio = (r - minLength) / rangeLength;
		float halfAngleRad = angle * 0.5f * 0.01745329251f;
		float sideRatio = isFullCircle ? 1.0f : (halfAngleRad - acos(clamp(cosTheta, -1.0f, 1.0f))) / max(halfAngleRad, 1e-5f);
		float edgeRatio = min(outerRatio, min(innerRatio, sideRatio));
		float edgeProgress = 1.0f - saturate(edgeRatio / 0.3f);
		float edgeFade = lerp(1.0f, 0.2f, pow(edgeProgress, 4.0f));
		float4 decalSample = decalTex.Sample(samp, float3(u, v, textureIndex));
		float fade = saturate(remaining / max(maxDraw, 1e-5f));
		float texAlpha = saturate(decalSample.a * decals[i].color.a * fade * edgeFade);
		// !!!New!!!
		float wobble = (FrameNoise(pin.wPos.xz * 3.0f) * 0.6f + FrameNoise(pin.wPos.xz * 9.0f) * 0.4f - 0.5f) * 0.2f;
		float frameProgress = 1.0f - saturate((edgeRatio + wobble) / 0.3f);
		float3 frameColor = float3(1.0f, 1.0f, 1.0f);
		if (frameProgress > 0.0f)
		{
			float3 frameSum = float3(0.0f, 0.0f, 0.0f);
			float frameWeight = 0.0f;
			[unroll]
			for (int fy = 0; fy < 4; ++fy)
			{
				[unroll]
				for (int fx = 0; fx < 4; ++fx)
				{
					float4 frameSample = decalTex.SampleLevel(samp, float3((fx + 0.5f) / 4.0f, (fy + 0.5f) / 4.0f, textureIndex), 0);
					frameSum += frameSample.rgb * frameSample.a;
					frameWeight += frameSample.a;
				}
			}
			if (frameWeight > 1e-5f)
			{
				frameColor = frameSum / frameWeight;
			}
		}
		// !!!New!!!
		float frameRise = pow(saturate(frameProgress / 0.9f), 5.0f);
		float frameTail = lerp(1.0f, 0.3f, saturate((frameProgress - 0.9f) / 0.1f));
		float frameBoost = lerp(1.0f, 1.5f, saturate((frameProgress - 0.92f) / 0.05f));
		float frameAlpha = saturate(decals[i].color.a * fade * 0.3f * frameRise * frameTail * frameBoost);
		float alpha = texAlpha + frameAlpha * (1.0f - texAlpha);
		float3 decalRgb = (decalSample.rgb * texAlpha + frameColor * frameAlpha * (1.0f - texAlpha)) / max(alpha, 1e-5f);
		float3 decalWhite = float3(1.0f, 1.0f, 1.0f);
		color.rgb = lerp(color.rgb, decalRgb * decalWhite, alpha);
	}
	return color;
})EOT";
	m_pPS[PS_ALPHA_SHADOW] = new PixelShader();
	m_pPS[PS_ALPHA_SHADOW]->Compile(code);
}