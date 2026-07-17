#include "DrawMatrix.h"

void DrawMatrix::CreateWorldMatrix(DirectX::XMFLOAT4X4 &a_fMat, const float3& a_fPos, const float3& a_fScale, const float3& a_fAngle, bool a_bTranspose, const float3& a_fOffset, bool a_bUseLocalOffset)
{
	// 各トランスフォームの行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(a_fScale.x, a_fScale.y, a_fScale.z);

	DirectX::XMMATRIX Rx = DirectX::XMMatrixRotationX(a_fAngle.x);
	DirectX::XMMATRIX Ry = DirectX::XMMatrixRotationY(a_fAngle.y);
	DirectX::XMMATRIX Rz = DirectX::XMMatrixRotationZ(a_fAngle.z);
	DirectX::XMMATRIX R = Rx * Ry * Rz;

	DirectX::XMFLOAT3 offsetF = { a_fOffset.x, a_fOffset.y, a_fOffset.z };
	if (a_bUseLocalOffset)
	{
		DirectX::XMVECTOR localOffset = DirectX::XMVectorSet(a_fOffset.x, a_fOffset.y, a_fOffset.z, 0.0f);
		DirectX::XMVECTOR worldOffset = DirectX::XMVector3TransformCoord(localOffset, R);
		DirectX::XMStoreFloat3(&offsetF, worldOffset);
	}

	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(
		a_fPos.x + offsetF.x,
		a_fPos.y + offsetF.y,
		a_fPos.z + offsetF.z);

	// 行列をかけ合わせる
	DirectX::XMMATRIX mat = S * R * T;
	// GPU用に配列を変換
	if (a_bTranspose) mat = DirectX::XMMatrixTranspose(mat);

	// 描画用の配列を代入
	DirectX::XMStoreFloat4x4(&a_fMat, mat);
}

void DrawMatrix::CreateWorldMatrix(DirectX::XMFLOAT4X4& a_fMat, const float3& a_fPos, const float3& a_fScale, const float3& a_fAngle, DirectX::XMMATRIX a_fBone, bool a_bTranspose, const float3& a_fOffset, bool a_bUseLocalOffset)
{
	// 各トランスフォームの行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(a_fScale.x, a_fScale.y, a_fScale.z);

	DirectX::XMMATRIX Rx = DirectX::XMMatrixRotationX(a_fAngle.x);
	DirectX::XMMATRIX Ry = DirectX::XMMatrixRotationY(a_fAngle.y);
	DirectX::XMMATRIX Rz = DirectX::XMMatrixRotationZ(a_fAngle.z);
	DirectX::XMMATRIX R = Rx * Ry * Rz;

	DirectX::XMFLOAT3 offsetF = { a_fOffset.x, a_fOffset.y, a_fOffset.z };
	if (a_bUseLocalOffset)
	{
		DirectX::XMVECTOR localOffset = DirectX::XMVectorSet(a_fOffset.x, a_fOffset.y, a_fOffset.z, 0.0f);
		DirectX::XMVECTOR worldOffset = DirectX::XMVector3TransformCoord(localOffset, R);
		DirectX::XMStoreFloat3(&offsetF, worldOffset);
	}

	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(
		a_fPos.x + offsetF.x,
		a_fPos.y + offsetF.y,
		a_fPos.z + offsetF.z);

	// 行列をかけ合わせる
	DirectX::XMMATRIX mat = S * R * T;

	// Boneの動きを掛け合わせる
	mat *= a_fBone;
	// GPU用に配列を変換
	if (a_bTranspose) mat = DirectX::XMMatrixTranspose(mat);

	// 描画用の配列を代入
	DirectX::XMStoreFloat4x4(&a_fMat, mat);
}



void DrawMatrix::CreateViewMatrix(DirectX::XMFLOAT4X4& a_fMat, const float3& a_fEyePosition, const float3& a_fFocusPosition, const float3& a_fUpDirection, bool a_bTranspose)
{
	DirectX::XMMATRIX view;

	// ビュー座標への変換行列
	view = DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorSet(a_fEyePosition.x, a_fEyePosition.y, a_fEyePosition.z, 0.0f),  // カメラの位置
		DirectX::XMVectorSet(a_fFocusPosition.x, a_fFocusPosition.y, a_fFocusPosition.z, 0.0f),  // カメラの注視点
		DirectX::XMVectorSet(a_fUpDirection.x, a_fUpDirection.y, a_fUpDirection.z, 0.0f)); // カメラにとっての上方向

	if (a_bTranspose) view = DirectX::XMMatrixTranspose(view);


	// GPU用の行列に変換
	DirectX::XMStoreFloat4x4(&a_fMat, view);
}

void DrawMatrix::CreateProjectionMatrix(DirectX::XMFLOAT4X4& a_fMat, const float a_fFovAngleY, const float AspectRatio, const float NearZ, const float FarZ, bool a_bTranspose)
{
	DirectX::XMMATRIX proj;

	// プロジェクション座標への変換行列
	proj = DirectX::XMMatrixPerspectiveFovLH(
		a_fFovAngleY * RAD,	// y軸の視野角
		AspectRatio,	// y軸の視野角からx軸の視野角を割り出すための割合
		NearZ,	// カメラの最小表示距離
		FarZ	// カメラの最大表示距離
	);

	if(a_bTranspose) proj = DirectX::XMMatrixTranspose(proj);

	DirectX::XMStoreFloat4x4(&a_fMat, proj);
}
