#include "Geometory.h"
#include "MathAssist.h"

#define MAX_VERTEX (4 * 6)
#define MAX_INDEX (6 * 6)

void Geometory::MakeBox()
{
	////--- 頂点の作成
	Vertex vtx[MAX_VERTEX] =
	{ 
		// -Z面
		{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}},
		{{ 0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}},
		{{-0.5f,-0.5f, -0.5f}, {0.0f, 1.0f}},
		{{ 0.5f,-0.5f, -0.5f}, {1.0f, 1.0f}},

		// Z面
		{{ 0.5f, 0.5f,  0.5f}, {0.0f, 0.0f}},
		{{-0.5f, 0.5f,  0.5f}, {1.0f, 0.0f}},
		{{ 0.5f,-0.5f,  0.5f}, {0.0f, 1.0f}},
		{{-0.5f,-0.5f,  0.5f}, {1.0f, 1.0f}},

		// -X面
		{{-0.5f, 0.5f,  0.5f}, {0.0f, 0.0f}},
		{{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}},
		{{-0.5f,-0.5f,  0.5f}, {0.0f, 1.0f}},
		{{-0.5f,-0.5f, -0.5f}, {1.0f, 1.0f}},

		// X面
		{{ 0.5f, 0.5f,  -0.5f},{0.0f, 0.0f}},
		{{ 0.5f, 0.5f,  0.5f}, {1.0f, 0.0f}},
		{{ 0.5f,-0.5f,  -0.5f},{0.0f, 1.0f}},
		{{ 0.5f,-0.5f,  0.5f}, {1.0f, 1.0f}},

		// -Y面
		{{  0.5f, -0.5f, -0.5f},{0.0f, 0.0f}},
		{{  0.5f, -0.5f,  0.5f},{1.0f, 0.0f}},
		{{ -0.5f, -0.5f, -0.5f},{0.0f, 1.0f}},
		{{ -0.5f, -0.5f,  0.5f},{1.0f, 1.0f}},

		// Y面
		{{ -0.5f, 0.5f, -0.5f},{0.0f, 0.0f}},
		{{ -0.5f, 0.5f,  0.5f},{1.0f, 0.0f}},
		{{  0.5f, 0.5f, -0.5f},{0.0f, 1.0f}},
		{{  0.5f, 0.5f,  0.5f},{1.0f, 1.0f}},
		
	};

	//--- インデックスの作成
	int idx[MAX_INDEX] =
	{
		0,1,2, 1,3,2, // -Z面
		4,5,6, 5,7,6,  //  Z面
		8,9,10, 9,11,10,
		12,13,14, 13,15,14,
		16,17,18, 17,19,18,
		20,21,22, 21,23,22,
	};

	// バッファの作成
	MeshBuffer::Description desc = {};
	desc.pVtx = vtx;
	desc.vtxCount = MAX_VERTEX;
	desc.vtxSize = sizeof(vtx[0]);
	desc.pIdx = idx;
	desc.idxCount = MAX_INDEX;
	desc.idxSize = sizeof(idx[0]);
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	m_pBox = new MeshBuffer();
	m_pBox->Create(desc);
	
	
	
}

void Geometory::MakeCylinder()
{
	//--- 頂点の作成
	// 天面、底面

	// 側面

	//--- インデックスの作成
	// 天面、底面

	// 側面


	//--- バッファの作成
}

void Geometory::MakeSphere()
{
	//--- 頂点の作成

	//--- インデックスの作成

	// バッファの作成
}

void Geometory::RegisterSector(std::string key, float minLength, float maxLength, float angle, float maxHeight, float maxLowness, int circumferenceCount)
{
	// mapに既に登録されている場合は作成しない
	if (m_sectorBuffers.find(key) != m_sectorBuffers.end()) return;

	// 頂点数を計算
	int vertexCount = 0;
	int outsideVertexCount = circumferenceCount;
	float insideRate = (maxLength - minLength) / maxLength;
	int insideVertexCount = max((int)(circumferenceCount * insideRate), 1);
	
	maxLowness *= -1.0f;

	vertexCount = (outsideVertexCount + insideVertexCount) * 2;

	// 配列を動的確保
	Vertex* vertexes = new Vertex[vertexCount];
	for (int i = 0; i < 2; i++)
	{
		bool heiVertex = i % 2 == 0;
		float height = heiVertex ? maxHeight : maxLowness;
		float uvHeight = heiVertex ? 0.0f : 1.0f;
		for (int j = 0; j < outsideVertexCount; j++)
		{
			// アングルから位置を取得
			float currentAngle = (angle / (circumferenceCount - 1)) * j - angle / 2;

			float2 vec = AngleToVector(-currentAngle + 90.0f);

			vertexes[j + i * outsideVertexCount] = { { vec.x * maxLength, height, vec.y * maxLength }, { (float)j / (float)outsideVertexCount, uvHeight } };

		}
	}

	for (int i = 0; i < 2; i++)
	{
		bool heiVertex = i % 2 == 0;
		float height = heiVertex ? maxHeight : maxLowness;
		float uvHeight = heiVertex ? 0.0f : 1.0f;
		for (int j = 0; j < insideVertexCount; j++)
		{
			// アングルから位置を取得
			float currentAngle = (angle / (insideVertexCount - 1)) * j - angle / 2;

			float2 vec = AngleToVector(-currentAngle + 90.0f);

			vertexes[j + i * insideVertexCount + outsideVertexCount * 2] = { { vec.x * minLength, height, vec.y * minLength }, { 1.0f - (float)j / (float)insideVertexCount, uvHeight } };
		}
	}



	// インデックス数を計算し配列を動的確保
	// sector: 上下面 / flont・back: 外弧・内弧の壁 / side: 左右端
	int sectorIndexCount = outsideVertexCount - 1 + insideVertexCount - 1;
	int flontIndexCount = (outsideVertexCount - 1) * 2;
	int backIndexCount = (insideVertexCount - 1) * 2;
	int sideIndexCount = 2 * 2;
	int indexCount = sectorIndexCount * 2 + flontIndexCount + backIndexCount + sideIndexCount;
	int* indexes = new int[indexCount * 3];
	// 扇を代入
	int indexProgress = 0;
	const int outerTopBase = 0;
	const int outerBottomBase = outsideVertexCount;
	const int innerTopBase = outsideVertexCount * 2;
	const int innerBottomBase = outsideVertexCount * 2 + insideVertexCount;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < outsideVertexCount - 1; j++)
		{
			float currentIndexRatio = (float)(j + 1.0f) / (float)(outsideVertexCount - 1); 

			int insideIndex = insideVertexCount - 1;
			for(int k = 0; k < insideVertexCount - 1; k++)
			{
				float insideRatio = (float)(k + 1) / (float)(insideVertexCount - 1);
				if(currentIndexRatio > insideRatio) continue;

				insideIndex = k;
				break;
			}
			float out0 = j + outsideVertexCount * i;
			float out1 = j + 1 + outsideVertexCount * i;
			float in0 = outsideVertexCount * 2 + insideIndex + insideVertexCount * i;

			indexes[j * 3 + (outsideVertexCount - 1) * 3 * i] = out1;
			indexes[j * 3 + 1 + (outsideVertexCount - 1) * 3 * i] = in0;
			indexes[j * 3 + 2 + (outsideVertexCount - 1) * 3 * i] = out0;
		}
	}

	indexProgress += (outsideVertexCount - 1) * 2 * 3;

	for(int i = 0; i < 2; i++)
	{
		for(int j = 0; j < insideVertexCount - 1 ; j++)
		{
			// 割合を出す
			float nextIndexRatio = (float)(j + 1)/ (float)(insideVertexCount - 1); 

			int outsideIndex = 1;
			for(int k = 0; k < outsideVertexCount; k++)
			{
				float currentOutsideRatio = (float)(k + 1) / (float)(outsideVertexCount - 1);
				if(nextIndexRatio >= currentOutsideRatio) continue;

				outsideIndex = k;
				break;
			}

			int in0 = j + insideVertexCount * i + outsideVertexCount * 2;
			int in1 = (j + 1) + insideVertexCount * i + outsideVertexCount * 2;
			int out0 = outsideIndex + outsideVertexCount * i;

			indexes[(j - 1) * 3 + (insideVertexCount - 1) * 3 * i + indexProgress] =  in0;
			indexes[(j - 1) * 3 + 1 + (insideVertexCount - 1) * 3 * i + indexProgress] = out0;
			indexes[(j - 1) * 3 + 2 + (insideVertexCount - 1) * 3 * i + indexProgress] = in1;
		}
	}

	indexProgress += (insideVertexCount - 1) * 2 * 3;

	// 扇の前後を代入
	for (int j = 0; j < outsideVertexCount - 1; j++)
	{
		const int t0 = outerTopBase + j;
		const int t1 = outerTopBase + j + 1;
		const int b0 = outerBottomBase + j;
		const int b1 = outerBottomBase + j + 1;
		const int base = indexProgress + j * 6;
		indexes[base + 0] = t0;
		indexes[base + 1] = b0;
		indexes[base + 2] = t1;
		indexes[base + 3] = t1;
		indexes[base + 4] = b0;
		indexes[base + 5] = b1;
	}
	indexProgress += flontIndexCount * 3;

	for (int j = 0; j < insideVertexCount - 1; j++)
	{
		const int t0 = innerTopBase + j;
		const int t1 = innerTopBase + j + 1;
		const int b0 = innerBottomBase + j;
		const int b1 = innerBottomBase + j + 1;
		const int base = indexProgress + j * 6;
		indexes[base + 0] = t0;
		indexes[base + 1] = t1;
		indexes[base + 2] = b0;
		indexes[base + 3] = t1;
		indexes[base + 4] = b1;
		indexes[base + 5] = b0;
	}
	indexProgress += backIndexCount * 3;

	// 扇の左右を代入
	{
		const int ot = outerTopBase;
		const int ob = outerBottomBase;
		const int it = innerTopBase;
		const int ib = innerBottomBase;
		indexes[indexProgress + 0] = ot;
		indexes[indexProgress + 1] = it;
		indexes[indexProgress + 2] = ob;
		indexes[indexProgress + 3] = it;
		indexes[indexProgress + 4] = ib;
		indexes[indexProgress + 5] = ob;
		indexProgress += 6;

		const int otR = outerTopBase + outsideVertexCount - 1;
		const int obR = outerBottomBase + outsideVertexCount - 1;
		const int itR = innerTopBase + insideVertexCount - 1;
		const int ibR = innerBottomBase + insideVertexCount - 1;
		indexes[indexProgress + 0] = otR;
		indexes[indexProgress + 1] = obR;
		indexes[indexProgress + 2] = itR;
		indexes[indexProgress + 3] = itR;
		indexes[indexProgress + 4] = obR;
		indexes[indexProgress + 5] = ibR;
		indexProgress += 6;
	}

	// バッファの作成
	MeshBuffer::Description desc = {};
	desc.pVtx = vertexes;
	desc.vtxCount = vertexCount;
	desc.vtxSize = sizeof(vertexes[0]);
	desc.pIdx = indexes;
	desc.idxCount = indexProgress;
	desc.idxSize = sizeof(indexes[0]);
	desc.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// mapに登録
	MeshBuffer* buff = new MeshBuffer();
	buff->Create(desc);
	m_sectorBuffers.insert({ key, buff });
	m_dynamicVertexSource.insert({ key, vertexes });
	m_dynamicIndexSource.insert({ key, indexes });
}
