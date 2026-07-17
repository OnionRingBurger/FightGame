#include "TrailEffect.h"

TrailEffect::TrailEffect(int maxVertices) : PolylineEffect(maxVertices)
{
}


void TrailEffect::AddUpdateLine(LineID a_id, TrailUpdateData a_deta)
{
	updateData.insert({ a_id, a_deta });
}

void TrailEffect::Update()
{
	updateData.clear();
}

void TrailEffect::UpdateControlPoints(LineID id, ControlPoints& controlPoints)
{
	UpdateDatas::iterator dataIt = updateData.find(id);
	if (dataIt == updateData.end()) return;
	dataIt;
	TrailUpdateData data = dataIt->second;

	ControlPoints::iterator it = controlPoints.begin();
	while (it != controlPoints.end())
	{
		it->bold *= data.widthShrinkage;
		++it;
	}

	float distance = 0.0f;
	
	DirectX::XMFLOAT3 pos(data.currentPos.x, data.currentPos.y, data.currentPos.z);
	DirectX::XMVECTOR vOld = DirectX::XMLoadFloat3(&controlPoints.at(1).pos);
	DirectX::XMVECTOR vNow = DirectX::XMLoadFloat3(&pos);
	DirectX::XMVECTOR vDir = DirectX::XMVectorSubtract(vNow, vOld);

	// ベクトルから距離を算出 
	DirectX::XMStoreFloat(&distance, DirectX::XMVector3Length(vDir));

	// プレイヤーが移動しているか判定
	if (distance >= 0.7f) 
	{ 
		// 先頭以外の制御点の情報を一つずつずらす 
		for (int i = controlPoints.size() - 1; i > 0; --i) 
		{
			controlPoints[i] = controlPoints[i - 1]; 
		}
	}

	// 先頭に新しいデータを設定 
	controlPoints[0].pos = pos; controlPoints[0].bold = data.widthSize;

}
