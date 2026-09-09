#ifndef __GEOMETORY_H__
#define __GEOMETORY_H__

#include <DirectXMath.h>
#include <unordered_map>
#include "Shader.h"
#include "MeshBuffer.h"
#include <string>
#include "MathAssist.h"

class Geometory
{
private:
	struct LineVertex
	{
		float pos[3];
		float color[4];
	};
	struct Vertex
	{
		float pos[3];
		float uv[2];
	};
	// !!!New!!!
	struct SectorShape
	{
		float minLength;
		float maxLength;
		float angle;
		float maxHeight;
		float maxLowness;
	};
public:
	static void Init();
	static void Uninit();

	static void SetWorld(DirectX::XMFLOAT4X4 world);
	static void SetView(DirectX::XMFLOAT4X4 view);
	static void SetProjection(DirectX::XMFLOAT4X4 proj);

	static void AddLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end,
		DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	static void DrawLines();
	static void DrawBox();
	static void DrawCylinder();
	static void DrawSphere();

	// éñëOÇ…êFÇÒÇ»å`ÇÃêÓÇìoò^Ç∑ÇÈ
	static void RegisterSector(std::string key ,float minLength, float maxLength, float angle, float maxHeight, float maxLowness, int circumferenceCount);
	// ìoò^Ç≥ÇÍÇƒÇ¢ÇÈêÓå`Çï`âÊÇ∑ÇÈ
	static void DrawSector(std::string key, float progress, float maxTime, float highlightTime, float3 defaultColor);

private:
	static void MakeVS();
	static void MakePS();
	static void MakeLineShader();
	static void MakeLine();
	static void MakeSectorShader();

private:
	static void MakeBox();
	static void MakeCylinder();
	static void MakeSphere();

private:
	static const int MAX_LINE_NUM = 1000;
	static const int CIRCLE_DETAIL = 16;
private:
	static MeshBuffer* m_pBox;
	static MeshBuffer* m_pCylinder;
	static MeshBuffer* m_pSphere;
	static MeshBuffer* m_pLines;
	static Shader* m_pVS;
	static Shader* m_pPS;
	static Shader* m_sectorVS;
	static Shader* m_sectorPS;
	static Shader* m_pLineShader[2];
	static DirectX::XMFLOAT4X4 m_WVP[3];
	static void* m_pLineVtx;
	static int m_lineCnt;
	static std::unordered_map<std::string, MeshBuffer*> m_sectorBuffers;
	// !!!New!!!
	static std::unordered_map<std::string, SectorShape> m_sectorShapes;
	static std::unordered_map<std::string, Vertex*> m_dynamicVertexSource;
	static std::unordered_map<std::string, int*> m_dynamicIndexSource;
};

#endif // __GEOMETORY_H__