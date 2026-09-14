#include "Geometory.h"

MeshBuffer* Geometory::m_pBox;
MeshBuffer* Geometory::m_pCylinder;
MeshBuffer* Geometory::m_pSphere;
MeshBuffer* Geometory::m_pLines;
Shader* Geometory::m_pVS;
Shader* Geometory::m_pPS;
Shader* Geometory::m_pLineShader[2];
DirectX::XMFLOAT4X4 Geometory::m_WVP[3];
void* Geometory::m_pLineVtx;
int Geometory::m_lineCnt = 0;

// 毎回忘れるStatic参照、複数ファイルに書くのもNG
std::unordered_map<std::string, MeshBuffer*> Geometory::m_sectorBuffers;
// !!!New!!!
std::unordered_map<std::string, Geometory::SectorShape> Geometory::m_sectorShapes;
std::unordered_map<std::string, Geometory::Vertex*> Geometory::m_dynamicVertexSource;
std::unordered_map<std::string, int*> Geometory::m_dynamicIndexSource;
Shader* Geometory::m_sectorVS;
Shader* Geometory::m_sectorPS;
// !!!New!!!
Shader* Geometory::m_attackSectorVS;
Shader* Geometory::m_attackSectorPS;

void Geometory::Init()
{
	for (int i = 0; i < 3; ++i)
		DirectX::XMStoreFloat4x4(&m_WVP[i], DirectX::XMMatrixIdentity());
	MakeBox();
	MakeCylinder();
	MakeSphere();
	MakeVS();
	MakePS();
	MakeLineShader();
	MakeLine();
	MakeSectorShader();
	// !!!New!!!
	MakeAttackSectorShader();
}
void Geometory::Uninit()
{
	SAFE_DELETE_ARRAY(m_pLineVtx);
	SAFE_DELETE(m_pLineShader[1]);
	SAFE_DELETE(m_pLineShader[0]);
	SAFE_DELETE(m_pPS);
	SAFE_DELETE(m_pVS);
	// !!!New!!!
	SAFE_DELETE(m_attackSectorPS);
	SAFE_DELETE(m_attackSectorVS);

	SAFE_DELETE(m_sectorPS);
	SAFE_DELETE(m_sectorVS);
	SAFE_DELETE(m_pLines);
	SAFE_DELETE(m_pSphere);
	SAFE_DELETE(m_pCylinder);
	SAFE_DELETE(m_pBox);
	
	for (auto it : m_sectorBuffers)
	{
		SAFE_DELETE(it.second);
	}
	m_sectorBuffers.clear();

	// !!!New!!!
	m_sectorShapes.clear();

	for (auto it : m_dynamicVertexSource)
	{
		SAFE_DELETE_ARRAY(it.second);
	}
	m_dynamicVertexSource.clear();

	for (auto it : m_dynamicIndexSource)
	{
		SAFE_DELETE_ARRAY(it.second);
	}
	m_dynamicIndexSource.clear();
	
}

void Geometory::SetWorld(DirectX::XMFLOAT4X4 world)
{
	m_WVP[0] = world;
}
void Geometory::SetView(DirectX::XMFLOAT4X4 view)
{
	m_WVP[1] = view;
}
void Geometory::SetProjection(DirectX::XMFLOAT4X4 proj)
{
	m_WVP[2] = proj;
}

void Geometory::AddLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color)
{
	if (m_lineCnt < MAX_LINE_NUM)
	{
		LineVertex* pVtx = reinterpret_cast<LineVertex*>(m_pLineVtx);
		pVtx[m_lineCnt * 2 + 0] = { start.x, start.y, start.z, color.x, color.y, color.z, color.w};
		pVtx[m_lineCnt * 2 + 1] = { end.x, end.y, end.z, color.x, color.y, color.z, color.w };
		++m_lineCnt;
	}
}
void Geometory::DrawLines()
{
	m_pLineShader[0]->WriteBuffer(0, m_WVP);
	m_pLineShader[0]->Bind();
	m_pLineShader[1]->Bind();
	m_pLines->Write(m_pLineVtx);
	m_pLines->Draw(m_lineCnt * 2);
	m_lineCnt = 0;
}



void Geometory::DrawBox()
{
	if (m_pBox == nullptr)
		return;
	m_pVS->WriteBuffer(0, m_WVP);
	m_pVS->Bind();
	m_pPS->Bind();
	m_pBox->Draw();
}
void Geometory::DrawCylinder()
{
	if (m_pCylinder == nullptr)
		return;
	m_pVS->WriteBuffer(0, m_WVP);
	m_pVS->Bind();
	m_pPS->Bind();
	m_pCylinder->Draw();
}
void Geometory::DrawSphere()
{
	if (m_pSphere == nullptr)
		return;
	m_pVS->WriteBuffer(0, m_WVP);
	m_pVS->Bind();
	m_pPS->Bind();
	m_pSphere->Draw();
}

void Geometory::DrawSector(std::string key, float progress, float maxTime, float highlightTime, float3 defaultColor, SectorShaderType shaderType)
{
	if (m_sectorBuffers.find(key) == m_sectorBuffers.end() || m_sectorBuffers.at(key) == nullptr) return;

	// !!!New!!!
	if (m_sectorShapes.find(key) == m_sectorShapes.end()) return;
	const SectorShape& shape = m_sectorShapes.at(key);

	// !!!New!!!
	Shader* sectorVS = m_sectorVS;
	Shader* sectorPS = m_sectorPS;
	if (shaderType == SectorShaderType::Attack)
	{
		sectorVS = m_attackSectorVS;
		sectorPS = m_attackSectorPS;
	}

	sectorVS->WriteBuffer(0, m_WVP);
	sectorVS->Bind();

	// !!!New!!!
	float pixelBuff[16] =
	{
		defaultColor.x, defaultColor.y, defaultColor.z, 0.0f,
		progress, maxTime, highlightTime, 0.0f,
		shape.minLength, shape.maxLength, shape.angle * 0.5f * RAD, 0.0f,
		shape.maxHeight, shape.maxLowness, 0.0f, 0.0f,
	};
	sectorPS->WriteBuffer(0, pixelBuff);
	sectorPS->Bind();
	m_sectorBuffers.at(key)->Draw();

}


void Geometory::MakeVS()
{
const char* VSCode = R"EOT(
struct VS_IN {
	float3 pos : POSITION0;
	float2 uv : TEXCOORD0;
};
struct VS_OUT {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};
cbuffer Matrix : register(b0) {
	float4x4 world;
	float4x4 view;
	float4x4 proj;
};
VS_OUT main(VS_IN vin) {
	VS_OUT vout;
	vout.pos = float4(vin.pos, 1.0f);
	vout.pos = mul(vout.pos, world);
	vout.pos = mul(vout.pos, view);
	vout.pos = mul(vout.pos, proj);
	vout.uv = vin.uv;
	return vout;
})EOT";

	m_pVS = new VertexShader();
	m_pVS->Compile(VSCode);
}

void Geometory::MakePS()
{
#ifdef _DEBUG
	const char* PSCode = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};
float4 main(PS_IN pin) : SV_TARGET0 {
	float4 color = float4(1,1,1,1);
	float2 halfGrid = floor(abs(pin.uv) * 2.0f);
	float2 quatGrid = floor(abs(pin.uv) * 8.0f);

	float half = fmod(halfGrid.x + halfGrid.y, 2.0f);
	float quat = fmod(quatGrid.x + quatGrid.y, 2.0f);

	color.rgb = ((half * 0.1f) * quat + 0.45f) + (1 - quat) * 0.05f;
	return color;
})EOT";
#else
	const char* PSCode = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};
float4 main(PS_IN pin) : SV_TARGET0 {
	return float4(1.0f, 0.0f, 1.0f, 1.0f);
})EOT";
#endif

	m_pPS = new PixelShader();
	m_pPS->Compile(PSCode);
}
void Geometory::MakeLineShader()
{
	const char* VSCode = R"EOT(
struct VS_IN {
	float3 pos : POSITION0;
	float4 color : COLOR0;
};
struct VS_OUT {
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
};
cbuffer Matrix : register(b0) {
	float4x4 world;
	float4x4 view;
	float4x4 proj;
};
VS_OUT main(VS_IN vin) {
	VS_OUT vout;
	vout.pos = float4(vin.pos, 1.0f);
	vout.pos = mul(vout.pos, view);
	vout.pos = mul(vout.pos, proj);
	vout.color = vin.color;
	return vout;
})EOT";

	const char* PSCode = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
};
float4 main(PS_IN pin) : SV_TARGET0 {
	return pin.color;
})EOT";

	m_pLineShader[0] = new VertexShader();
	m_pLineShader[0]->Compile(VSCode);
	m_pLineShader[1] = new PixelShader();
	m_pLineShader[1]->Compile(PSCode);
}
void Geometory::MakeLine()
{
	m_pLineVtx = new LineVertex[MAX_LINE_NUM * 2];
	m_lineCnt = 0;
	MeshBuffer::Description desc = {};
	desc.pVtx = m_pLineVtx;
	desc.vtxCount = MAX_LINE_NUM * 2;
	desc.vtxSize = sizeof(LineVertex);
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	desc.isWrite = true;
	m_pLines = new MeshBuffer();
	m_pLines->Create(desc);
}

void Geometory::MakeSectorShader()
{
	const char* VSCode = R"EOT(
struct VS_IN {
	float3 pos : POSITION0;
	float2 uv : TEXCOORD0;
};
struct VS_OUT {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 local : TEXCOORD1;
};
cbuffer Matrix : register(b0) {
	float4x4 world;
	float4x4 view;
	float4x4 proj;
};
VS_OUT main(VS_IN vin) {
	VS_OUT vout;
	vout.pos = float4(vin.pos, 1.0f);
	vout.pos = mul(vout.pos, world);
	vout.pos = mul(vout.pos, view);
	vout.pos = mul(vout.pos, proj);
	vout.uv = vin.uv;
	// !!!New!!!
	vout.local = vin.pos;
	return vout;
})EOT";

	m_sectorVS = new VertexShader();
	m_sectorVS->Compile(VSCode);

	const char* PSCode = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 local : TEXCOORD1;
};
cbuffer SectorInfo : register(b0){
	float4 sectorColor;
	float4 sectorTime;
	float4 sectorShape;
	float4 sectorHeight;
};
float EdgeLine(float dist, float width, float grad) {
	if (grad <= 0.000001f) return 0.0f;
	return 1.0f - smoothstep(0.0f, grad * width, dist);
}
float4 main(PS_IN pin) : SV_TARGET0 {
	// !!!New!!!
	float minLength = sectorShape.x;
	float maxLength = max(sectorShape.y, minLength + 0.0001f);
	float halfAngle = max(sectorShape.z, 0.0001f);
	float radius = length(pin.local.xz);
	float radial = saturate((radius - minLength) / (maxLength - minLength));
	float angular = clamp(atan2(pin.local.x, pin.local.z) / halfAngle, -1.0f, 1.0f);
	float heightRange = max(sectorHeight.x + sectorHeight.y, 0.0001f);
	float vertical = saturate((pin.local.y + sectorHeight.y) / heightRange);

	// !!!New!!!
	float radialGrad = fwidth(radial);
	float angularGrad = fwidth(angular);
	float verticalGrad = fwidth(vertical);
	float radialLine = EdgeLine(min(radial, 1.0f - radial), 1.5f, radialGrad);
	float angularLine = EdgeLine(1.0f - abs(angular), 1.5f, angularGrad);
	float verticalLine = EdgeLine(min(vertical, 1.0f - vertical), 1.5f, verticalGrad);

	// !!!New!!!
	float suppressSideOutline = step(3.12f, halfAngle);
	angularLine *= (1.0f - suppressSideOutline);

	float outline = saturate(max(max(radialLine, angularLine), verticalLine));

	// !!!New!!!
	float progress = saturate(sectorTime.x);
	float remainTime = max(sectorTime.y, 0.0f) * (1.0f - progress);
	float highlight = 0.0f;
	if (sectorTime.z > 0.0f) highlight = saturate(1.0f - remainTime / sectorTime.z);
	float blink = 0.5f + 0.5f * sin(remainTime * 30.0f);
	float highlightPulse = highlight * (0.65f + 0.35f * blink);

	// !!!New!!!
	float sweepWidth = max(radialGrad * 2.0f, 0.015f);
	float sweepLine = 1.0f - smoothstep(0.0f, sweepWidth, abs(radial - progress));
	float trail = step(radial, progress) * saturate(1.0f - (progress - radial) / 0.4f);
	float ringPhase = frac(radial * 3.0f - progress * 3.0f);
	float echo = (1.0f - smoothstep(0.0f, 0.08f, ringPhase)) * 0.35f;
	echo *= step(0.000001f, radialGrad);

	// !!!New!!!
	float angularFade = 1.0f - pow(abs(angular), 3.0f);
	// color.a = min(0.6f , progress * 0.6f + 0.05); !!!Old!!!
	float baseAlpha = lerp(0.30f, 0.62f, pow(radial, 1.5f)) * lerp(0.78f, 1.0f, angularFade);

	// !!!New!!!
	float3 highlightColor = float3(1.0f, 0.92f, 0.72f);
	float3 baseColor = sectorColor.rgb;
	float3 color = baseColor * (1.45f + 0.7f * radial) + 0.12f;
	color += baseColor * trail * 0.95f;
	color += baseColor * echo * 1.15f;
	color = lerp(color, highlightColor, highlightPulse * 0.65f);
	color += lerp(baseColor, highlightColor, highlightPulse) * sweepLine * 1.9f;
	float3 outlineColor = lerp(saturate(baseColor * 2.2f + 0.4f), highlightColor, highlightPulse);
	color = lerp(color, outlineColor, outline);

	// !!!New!!!
	float alpha = baseAlpha + trail * 0.22f + echo * 0.16f + sweepLine * 0.55f;
	alpha = lerp(alpha, saturate(alpha + 0.38f), highlightPulse);
	alpha = max(alpha, outline * lerp(0.78f, 1.0f, highlightPulse));

	return float4(saturate(color), saturate(alpha));
})EOT";

	m_sectorPS = new PixelShader();
	m_sectorPS->Compile(PSCode);
}

// !!!New!!!
void Geometory::MakeAttackSectorShader()
{
	const char* VSCode = R"EOT(
struct VS_IN {
	float3 pos : POSITION0;
	float2 uv : TEXCOORD0;
};
struct VS_OUT {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 local : TEXCOORD1;
};
cbuffer Matrix : register(b0) {
	float4x4 world;
	float4x4 view;
	float4x4 proj;
};
VS_OUT main(VS_IN vin) {
	VS_OUT vout;
	vout.pos = float4(vin.pos, 1.0f);
	vout.pos = mul(vout.pos, world);
	vout.pos = mul(vout.pos, view);
	vout.pos = mul(vout.pos, proj);
	vout.uv = vin.uv;
	vout.local = vin.pos;
	return vout;
})EOT";

	m_attackSectorVS = new VertexShader();
	m_attackSectorVS->Compile(VSCode);

	/*
	const char* PSCodeDot = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 local : TEXCOORD1;
};
cbuffer SectorInfo : register(b0){
	float4 sectorColor;
	float4 sectorTime;
	float4 sectorShape;
	float4 sectorHeight;
};
float Hash12(float2 p) {
	float3 v = frac(float3(p.xyx) * float3(0.1031f, 0.1030f, 0.0973f));
	v += dot(v, v.yzx + 33.33f);
	return frac((v.x + v.y) * v.z);
}
float DotLayer(float2 gridPos, float seed, float presence, float minRadius, float maxRadius) {
	float2 cell = floor(gridPos);
	float2 inCell = frac(gridPos) - 0.5f;
	float lit = step(Hash12(cell + seed), presence);
	float radius = lerp(minRadius, maxRadius, Hash12(cell + seed + 7.31f));
	float2 offset = float2(Hash12(cell + seed + 2.13f), Hash12(cell + seed + 5.71f)) - 0.5f;
	float2 center = offset * 2.0f * max(0.5f - radius, 0.0f);
	float2 local = abs(inCell - center);
	float dist = max(local.x, local.y);
	return lit * (1.0f - smoothstep(radius * 0.85f, radius, dist));
}
float4 main(PS_IN pin) : SV_TARGET0 {
	float minLength = sectorShape.x;
	float maxLength = max(sectorShape.y, minLength + 0.0001f);
	float halfAngle = max(sectorShape.z, 0.0001f);
	float radius = length(pin.local.xz);
	float radial = saturate((radius - minLength) / (maxLength - minLength));
	float angular = clamp(atan2(pin.local.x, pin.local.z) / halfAngle, -1.0f, 1.0f);
	float heightRange = max(sectorHeight.x + sectorHeight.y, 0.0001f);
	float vertical = saturate((pin.local.y + sectorHeight.y) / heightRange);
	float progress = saturate(sectorTime.x);
	float duration = max(sectorTime.y, 0.0001f);
	float elapsed = duration * progress;
	float slowStart = 0.65f;
	float slowScale = 0.35f;
	float headTime = duration * slowStart;
	float tailTime = max(duration * (1.0f - slowStart), 0.0001f);
	float over = max(elapsed - headTime, 0.0f);
	float phase = elapsed - (1.0f - slowScale) * over * over / (2.0f * tailTime);
	float flickerHz = 18.0f;
	float tick = floor(phase * flickerHz);
	float impact = pow(1.0f - progress, 1.5f);
	float2 jitter = float2(Hash12(float2(tick, 3.7f)), Hash12(float2(tick, 9.1f))) - 0.5f;
	float2 cellUV = float2(radial, angular * 0.5f + 0.5f) + jitter * lerp(0.06f, 0.5f, impact);
	float presence = lerp(0.38f, 0.52f, impact);
	float fineDots = DotLayer(cellUV * 14.0f, tick * 1.37f, presence, 0.19f, 0.36f);
	float coarseDots = DotLayer(cellUV * 6.0f, tick * 3.91f + 41.0f, 0.14f, 0.16f, 0.30f);
	float twinkle = lerp(0.88f, 1.0f, Hash12(float2(tick, 17.3f)));
	float sparkle = saturate(max(fineDots, coarseDots)) * twinkle;
	float3 flashColor = float3(1.0f, 1.0f, 1.0f);
	float3 baseColor = sectorColor.rgb;
	float3 color = saturate(baseColor * (2.4f + 1.0f * radial));
	color = lerp(color, flashColor, sparkle * lerp(0.8f, 1.0f, impact));
	float bodyAlpha = lerp(0.5f, 0.9f, impact) * lerp(0.8f, 1.0f, 1.0f - abs(angular));
	float alpha = bodyAlpha + sparkle * lerp(0.8f, 1.0f, impact);
	alpha *= lerp(0.95f, 1.0f, vertical);
	alpha *= lerp(1.0f, 0.6f, progress);
	return float4(saturate(color), saturate(alpha));
})EOT";
	*/

	const char* PSCode = R"EOT(
struct PS_IN {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 local : TEXCOORD1;
};
cbuffer SectorInfo : register(b0){
	float4 sectorColor;
	float4 sectorTime;
	float4 sectorShape;
	float4 sectorHeight;
};
float EdgeLine(float dist, float width, float grad) {
	if (grad <= 0.000001f) return 0.0f;
	return 1.0f - smoothstep(0.0f, grad * width, dist);
}
float4 main(PS_IN pin) : SV_TARGET0 {
	// !!!New!!!
	float minLength = sectorShape.x;
	float maxLength = max(sectorShape.y, minLength + 0.0001f);
	float halfAngle = max(sectorShape.z, 0.0001f);
	float radius = length(pin.local.xz);
	float radial = saturate((radius - minLength) / (maxLength - minLength));
	float angular = clamp(atan2(pin.local.x, pin.local.z) / halfAngle, -1.0f, 1.0f);
	float heightRange = max(sectorHeight.x + sectorHeight.y, 0.0001f);
	float vertical = saturate((pin.local.y + sectorHeight.y) / heightRange);

	// !!!New!!!
	float radialGrad = fwidth(radial);
	float angularGrad = fwidth(angular);
	float verticalGrad = fwidth(vertical);
	float radialLine = EdgeLine(min(radial, 1.0f - radial), 1.5f, radialGrad);
	float angularLine = EdgeLine(1.0f - abs(angular), 1.5f, angularGrad);
	float verticalLine = EdgeLine(min(vertical, 1.0f - vertical), 1.5f, verticalGrad);

	// !!!New!!!
	float suppressSideOutline = step(3.12f, halfAngle);
	angularLine *= (1.0f - suppressSideOutline);

	float outline = saturate(max(max(radialLine, angularLine), verticalLine));

	// !!!New!!!
	float progress = saturate(sectorTime.x);
	float duration = max(sectorTime.y, 0.0001f);
	float elapsed = duration * progress;
	float spatial = radial * 5.5f + angular * 3.2f + vertical * 2.1f;
	float pulseFast = sin(elapsed * 22.0f + spatial);
	float pulseMid = sin(elapsed * 14.0f - spatial * 0.7f);
	float pulseSlow = sin(elapsed * 8.0f + spatial * 0.35f);
	float wave = pulseFast * 0.2f + pulseMid * 0.45f + pulseSlow * 0.35f;
	float blink = saturate(wave * 0.5f + 0.5f);
	blink = lerp(0.86f, 1.0f, blink);

	// !!!New!!!
	float impact = pow(1.0f - progress, 1.5f);
	float angularWeight = lerp(0.8f, 1.0f, 1.0f - abs(angular));
	float bodyAlpha = lerp(0.5f, 0.9f, impact) * angularWeight;
	float sparkleWeight = lerp(0.8f, 1.0f, impact);

	// !!!New!!!
	float3 flashColor = float3(1.0f, 1.0f, 1.0f);
	float3 baseColor = sectorColor.rgb;
	float3 color = saturate(baseColor * (2.4f + 1.0f * radial));
	color *= blink;
	color = lerp(color, flashColor, saturate(blink - 0.68f) * 0.72f * sparkleWeight);
	color += flashColor * saturate(blink - 0.88f) * 0.1f * sparkleWeight;
	float3 outlineColor = saturate(baseColor * 2.0f + 0.35f);
	color = lerp(color, outlineColor, outline);

	// !!!New!!!
	float alpha = bodyAlpha * lerp(0.92f, 1.0f, blink);
	alpha += saturate(blink - 0.70f) * sparkleWeight * 0.28f;
	alpha = max(alpha, outline * lerp(0.82f, 1.0f, blink));
	alpha *= lerp(0.95f, 1.0f, vertical);
	alpha *= lerp(1.0f, 0.6f, progress);

	return float4(saturate(color), saturate(alpha));
})EOT";

	m_attackSectorPS = new PixelShader();
	m_attackSectorPS->Compile(PSCode);
}
