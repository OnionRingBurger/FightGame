#include "DrawSystem.h"
#include "Components.h"
#include "DrawMatrix.h"
#include "ShaderList.h"
#include "Sprite.h"
#include "GameData.h"

using namespace Component;

void OBBDraw(Chunk& a_chunk, const SystemContext& a_context, ComponentView cameraView);
void SectorDraw(Chunk& a_chunk, const SystemContext& a_context, ComponentView cameraView);
void CameraDraw(Chunk& a_chunk, const SystemContext& a_context, ComponentView cameraView, ComponentView entityView);
void SpriteDraw(Chunk& a_chunk, const SystemContext& a_context);
void PolylineDraw(Chunk& a_chunk, const SystemContext& a_context);
void UIDraw(Chunk& a_chunk, const SystemContext& a_context);
DirectX::XMMATRIX CreateBillBoardMatrix(const float3& cameraPos, const float3& lookPos, const float3& upVector);

void DrawSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView entityView = a_chunk.GetView<ComponentTypes<ModelKey, Position, Rotation, Scale>>();
	ComponentView cameraView = a_chunk.GetView<ComponentTypes<Position, Camera>>();
	CameraDraw(a_chunk, a_context, cameraView, entityView);

	ComponentView oBBCameraView = a_chunk.GetView<ComponentTypes<Position, Camera>>();
	OBBDraw(a_chunk, a_context, oBBCameraView);
	SectorDraw(a_chunk, a_context, oBBCameraView);

	SpriteDraw(a_chunk, a_context);

	//PolylineDraw(a_chunk, a_context);

	UIDraw(a_chunk, a_context);
}


void CameraDraw(Chunk& a_chunk, const SystemContext& a_context, ComponentView cameraView, ComponentView entityView)
{
	DirectX::XMFLOAT4X4 wvp[3];
	ComponentHandle<Camera> useCamera;
	ComponentHandle<Position> cameraPosition;
	ComponentHandle<Rotation> cameraRotation;
	ComponentHandle<ZoomComponent> zoom;
	// 一番優先度の高いカメラを取得

	auto it = cameraView.begin();
	auto end = cameraView.end();

	if (it == end) return;

	for (; it != end; ++it)
	{
		ComponentHandle<Camera> itCamera = a_chunk.GetComponent<Camera>(*it);

		if (!useCamera.IsValid() || (itCamera.Look().cameraPriority > useCamera.Look().cameraPriority))
		{
			useCamera = itCamera;
			cameraPosition = a_chunk.GetComponent<Position>(*it);
			cameraRotation = a_chunk.GetComponent<Rotation>(*it);
			zoom = a_chunk.GetComponent<ZoomComponent>(*it);
		}
	}
	// カメラがなかった場合抜ける
	if (!useCamera.IsValid() || !cameraPosition.IsValid() || !cameraRotation.IsValid()) return;

	float fovy = useCamera->fovy;
	float aspect = useCamera->aspect;
	float nearCrip = useCamera->nearCrip;
	float farCrip = useCamera->farCrip;
	float3 cameraForward = GetForward(TOFLOAT3(cameraRotation));

	if (zoom.IsValid())
	{
		if (zoom.Look().zoom > 1.0f) fovy /= zoom.Look().zoom;
	}

	DrawMatrix::CreateViewMatrix(wvp[1], float3{ cameraPosition->x, cameraPosition->y, cameraPosition->z }, useCamera->lookPosition, useCamera->upVector);
	DrawMatrix::CreateProjectionMatrix(wvp[2], fovy, aspect, nearCrip, farCrip);

	// ゴーストエリアの追加
	ComponentView areaView = a_chunk.GetView<ComponentTypes<Position, GhostAreaComponent>>();
	ShaderList::GhostAreas areas;
	for (auto it : areaView)
	{
		ComponentHandle<GhostAreaComponent> area = a_chunk.GetComponent<GhostAreaComponent>(it);
		ComponentHandle<Position> areaPos = a_chunk.GetComponent<Position>(it);
		ShaderList::GhostArea useArea(areaPos.Look().x, areaPos.Look().y, areaPos.Look().z, area.Look().length);
		areas.push_back(useArea);
		
	}
	ShaderList::SetCamera({ cameraPosition.Look().x, cameraPosition.Look().y, cameraPosition.Look().z }, {cameraForward.x, cameraForward.y, cameraForward.z });
	ShaderList::SetArea(areas);
	ShaderList::SetGhostFade(kGhostFadeRange);


	for (auto it : entityView)
	{
		const ComponentHandle<Position> pos = a_chunk.GetComponent<Position>(it);
		const ComponentHandle<Rotation> rot = a_chunk.GetComponent<Rotation>(it);
		const ComponentHandle<Scale> scale = a_chunk.GetComponent<Scale>(it);
		const ComponentHandle<ModelKey> key = a_chunk.GetComponent<ModelKey>(it);

		// 描画関連のフラグ
		const BitFlag flag = key.Look().modelDrawFlag;

		const bool useLocalOffset = (flag & MODEL_DRAW_LOCAL_OFFSET) != 0;
		float3 floatPos(pos.Look().x, pos.Look().y, pos.Look().z);
		float3 floatRot(
			(rot.Look().pitch + key.Look().rotato.x) * RAD
			, (rot.Look().yaw + key.Look().rotato.y) * RAD
			, (rot.Look().roll + key.Look().rotato.z) * RAD
		);
		float3 floatScale(scale.Look().x, scale.Look().y, scale.Look().z);

		DrawMatrix::CreateWorldMatrix(
			wvp[0], floatPos, floatScale, floatRot, true,
			key.Look().offset, useLocalOffset);

		ComponentHandle<AlphaBlendComponent> alphaBlend = a_chunk.GetComponent<AlphaBlendComponent>(it);

		// Modelの取得
		std::shared_ptr<Model> itModel = a_context.modelCache.GetModel(key.Look().key);
		if (!itModel)
		{
			continue;
		}
		bool isGhost = a_chunk.GetComponent<EnemyTag>(it).IsValid() || a_chunk.GetComponent<EnemyBulletTag>(it).IsValid();
		bool isAlpha = alphaBlend.IsValid();

		ShaderList::VSKind vsKind = key.Look().useAnime ? ShaderList::VS_ANIME : ShaderList::VS_WORLD;
		ShaderList::PSKind psKind;
		if (isGhost) psKind = ShaderList::PS_GHOST;
		else if (isAlpha) psKind = ShaderList::PS_ALPHA;
		else psKind = ShaderList::PS_SPECULAR;
		// シェーダーの登録
		ShaderList::SetWVP(wvp);
		itModel->SetVertexShader(ShaderList::GetVS(vsKind));
		itModel->SetPixelShader(ShaderList::GetPS(psKind));

		if (key.Look().modelDrawFlag & MODEL_DRAW_NOTDEPTH)
		{
			SetDepthTest(DEPTH_TEST_FALSE);
		}
		else
		{
			SetDepthTest(DEPTH_TEST_TRUE);
		}
		// 描画処理
		for (int i = 0; i < itModel->GetMeshNum(); i++)
		{
			// メッシュを取得
			Model::Mesh mesh = *itModel->GetMesh(i);

			DirectX::XMFLOAT4X4 bones[200];
			for (int boneIdx = 0; boneIdx < mesh.bones.size(); ++boneIdx)
			{
				const Model::Bone& bone = mesh.bones[boneIdx];
				DirectX::XMStoreFloat4x4(
					&bones[boneIdx],
					DirectX::XMMatrixTranspose(bone.invOffset * itModel->GetBoneMatrix(bone.nodeIndex)));
			} ShaderList::SetBones(bones);


			// 対応するマテリアルの登録
			Model::Material material = *itModel->GetMaterial(mesh.materialID);
			if (!(flag & MODEL_DRAW_SHADOW))
			{

				material.ambient.x = 1.0f;
				material.ambient.y = 1.0f;
				material.ambient.z = 1.0f;
			}

			ShaderList::SetMaterial(material);

			// 描画
			if (itModel)
			{
				if (alphaBlend.IsValid())
				{
					auto texture1 = a_context.uiCache.GetTexture(alphaBlend.Look().textureKey1);
					auto texture2 = a_context.uiCache.GetTexture(alphaBlend.Look().textureKey2);
					itModel->DrawAlphaBlend(texture1.get(), texture2.get());
				}
				else
				{
					itModel->Draw();
				}
				
			}
		}
	}

	SetDepthTest(DEPTH_TEST_TRUE);
}

// TODO UIだけでなくSpriteとしても描画できるようにする
void SpriteDraw(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentHandle<Camera> cameraHandle;
	ComponentHandle<Position> cameraPosHandle;
	ComponentView cameraView = a_chunk.GetView<ComponentTypes<Camera>>();


	int currentPriority = 0;
	for (auto it : cameraView)
	{
		const ComponentHandle<Camera> itCamera = a_chunk.GetComponent<Camera>(it);
		if (itCamera.Look().cameraPriority <= currentPriority) continue;

		cameraHandle = itCamera;
		cameraPosHandle = a_chunk.GetComponent<Position>(it);
		currentPriority = itCamera.Look().cameraPriority;


	}

	if (!cameraHandle.IsValid())
	{
		return;
	}

	ComponentView view = a_chunk.GetView<ComponentTypes<SpriteComponent, Position, Scale, Rotation>>();
	for (auto it : view)
	{
		const ComponentHandle<UIComponent> ui = a_chunk.GetComponent<UIComponent>(it);
		const ComponentHandle<SpriteComponent> sprite = a_chunk.GetComponent<SpriteComponent>(it);

		std::shared_ptr<Texture> uiTexture = a_context.uiCache.GetTexture(ui.Look().key);

		DirectX::XMFLOAT4X4 worldMat, viewMat, projMat;


		const float3 pos = sprite.Look().offsetPos +
			reinterpret_cast<const float3&>(a_chunk.GetComponent<Position>(it).Look());
		const float3 scale = reinterpret_cast<const float3&>(a_chunk.GetComponent<Scale>(it).Look());
		const float3 rotation = reinterpret_cast<const float3&>(a_chunk.GetComponent<Rotation>(it).Look()) +
			sprite.Look().offsetRotation + float3(0.0f, 0.0f, ui.Look().uiRotation);
		const float2 offset = ui.Look().uiPos;
		const float2 uvPos = ui.Look().uvPos;
		const float2 uvScale = ui.Look().uvScale;

		const float2 size = ui.Look().uiScale;

		float3 cameraPos(
			cameraPosHandle.Look().x,
			cameraPosHandle.Look().y,
			cameraPosHandle.Look().z
		);
		float3 lookPos = cameraHandle.Look().lookPosition;
		float3 upVector = cameraHandle.Look().upVector;

		DrawMatrix::CreateViewMatrix(viewMat, cameraPos, lookPos, upVector);
		DrawMatrix::CreateProjectionMatrix(projMat, cameraHandle.Look().fovy, cameraHandle.Look().aspect, cameraHandle.Look().farCrip, cameraHandle.Look().nearCrip);



		if (sprite.Look().isBillBoard)
		{
			// 各トランスフォームの行列を作成
			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

			DirectX::XMMATRIX Rz = DirectX::XMMatrixRotationZ(rotation.z);
			DirectX::XMMATRIX Bill = CreateBillBoardMatrix(
				reinterpret_cast<const float3&>(cameraPos),
				cameraHandle.Look().lookPosition,
				cameraHandle.Look().upVector
			);

			// 行列をかけ合わせる
			DirectX::XMMATRIX mat = S * Bill * T;

			mat = DirectX::XMMatrixTranspose(mat);

			// 描画用の配列を代入
			DirectX::XMStoreFloat4x4(&worldMat, mat);
		}
		else
		{

			DrawMatrix::CreateWorldMatrix(
				worldMat,
				pos,
				scale,
				rotation
			);

		}
		Sprite::SetWorld(worldMat);
		Sprite::SetView(viewMat);
		Sprite::SetProjection(projMat);
		Sprite::SetColor({ 1.0f, 1.0f, 1.0f,  ui.Look().alpha });
		Sprite::SetSize({ size.x, size.y});
		Sprite::SetOffset({ offset.x, offset.y });
		Sprite::SetTexture(uiTexture.get());
		Sprite::SetUVPos({ uvPos.x, uvPos.y });
		Sprite::SetUVScale({ uvScale.x, uvScale.y });

		Sprite::Draw();

	}


}


void UIDraw(Chunk& a_chunk, const SystemContext& a_context)
{
	SetDepthTest(DEPTH_TEST_FALSE);

	ComponentView view = a_chunk.GetView<ComponentTypes<UIComponent>, ComponentTypes<SpriteComponent>>();
	for (auto it : view)
	{
		const ComponentHandle<UIComponent> ui = a_chunk.GetComponent<UIComponent>(it);
		// アクティブじゃなかったら抜ける
		if (!ui.Look().isActiv) continue;

		std::shared_ptr<Texture> uiTexture = a_context.uiCache.GetTexture(ui.Look().key);

		DirectX::XMFLOAT4X4 worldMat, viewMat, projMat;

		float2 pos = ui.Look().uiPos;
		float2 size = ui.Look().uiScale;
		float rad = -ui.Look().uiRotation * RAD;
		float2 uvPos = ui.Look().uvPos;
		float2 uvScale = ui.Look().uvScale;
		DirectX::XMFLOAT2 dxPos = { pos.x, pos.y };
		DirectX::XMFLOAT2 dxSize = { size.x, size.y };
		DirectX::XMFLOAT2 dxUVPos = { uvPos.x, uvPos.y };
		DirectX::XMFLOAT2 dxUVScale = { uvScale.x, uvScale.y };

		float3 cameraPos = float3(0.0f, 0.0f, 0.0f);
		float3 lookPos = float3(0.0f, 0.0f, 1.0f);
		float3 upVector = float3(0.0f, 1.0f, 0.0f);




		DrawMatrix::CreateWorldMatrix(
			worldMat,
			float3(0.0f, 0.0f, 1.0f),
			float3(1.0f, 1.0f, 1.0f),
			float3(0.0f, 0.0f, rad)
		);

		DirectX::XMStoreFloat4x4(&viewMat, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&projMat, DirectX::XMMatrixIdentity());

		Sprite::SetWorld(worldMat);
		Sprite::SetView(viewMat);
		Sprite::SetProjection(projMat);
		Sprite::SetColor({ 1.0f, 1.0f, 1.0f,  ui.Look().alpha });
		Sprite::SetSize(dxSize);
		Sprite::SetOffset(dxPos);
		Sprite::SetTexture(uiTexture.get());
		Sprite::SetUVPos(dxUVPos);
		Sprite::SetUVScale(dxUVScale);

		Sprite::Draw();

	}

	SetDepthTest(DEPTH_TEST_TRUE);

}

void PolylineDraw(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentHandle<Camera> cameraHandle;
	ComponentHandle<Position> cameraPosHandle;
	ComponentView cameraView = a_chunk.GetView<ComponentTypes<Camera>>();


	int currentPriority = 0;
	for (auto it : cameraView)
	{
		const ComponentHandle<Camera> itCamera = a_chunk.GetComponent<Camera>(it);
		if (itCamera.Look().cameraPriority <= currentPriority) continue;

		cameraHandle = itCamera;
		cameraPosHandle = a_chunk.GetComponent<Position>(it);
		currentPriority = itCamera.Look().cameraPriority;


	}

	if (!cameraHandle.IsValid())
	{
		return;
	}

	DirectX::XMFLOAT4X4 vp[2];

	DrawMatrix::CreateViewMatrix(vp[0], float3{ cameraPosHandle->x, cameraPosHandle->y, cameraPosHandle->z }, cameraHandle->lookPosition, cameraHandle->upVector);
	DrawMatrix::CreateProjectionMatrix(vp[1], cameraHandle->fovy, cameraHandle->aspect, cameraHandle->nearCrip, cameraHandle->farCrip);


	a_context.trailEffect->SetView(vp[0]);
	a_context.trailEffect->SetProjection(vp[1]);
	a_context.trailEffect->SetTexture(a_context.uiCache.GetTexture("WhiteFadeBack").get());

	SetDepthTest(DEPTH_TEST_FALSE);
	a_context.trailEffect->Draw();
	SetDepthTest(DEPTH_TEST_TRUE);
}


DirectX::XMMATRIX CreateBillBoardMatrix(const float3& cameraPos, const float3& lookPos, const float3& upVector)
{
	// View行列を作成
	DirectX::XMMATRIX viewMat;

	// ビュー座標への変換行列
	viewMat = DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorSet(cameraPos.x, cameraPos.y, cameraPos.z, 0.0f),  // カメラの位置
		DirectX::XMVectorSet(lookPos.x, lookPos.y, lookPos.z, 0.0f),  // カメラの注視点
		DirectX::XMVectorSet(upVector.x, upVector.y, upVector.z, 0.0f)); // カメラにとっての上方向

	// cameraMatをXMMATRIXに変換して逆行列を作成
	DirectX::XMMATRIX cameraInvMat = DirectX::XMMatrixInverse(nullptr, viewMat);

	// 逆行列の移動を打ち消すために4X4に変換
	DirectX::XMFLOAT4X4 cameraInvCmp;
	DirectX::XMStoreFloat4x4(&cameraInvCmp, cameraInvMat);
	cameraInvCmp._41 = cameraInvCmp._42 = cameraInvCmp._43 = 0.0f;

	return DirectX::XMLoadFloat4x4(&cameraInvCmp);
}

void OBBDraw(Chunk& a_chunk, const SystemContext& a_context, ComponentView cameraView)
{
	DirectX::XMFLOAT4X4 wvp[3];
	ComponentHandle<Camera> useCamera;
	ComponentHandle<Position> cameraPosition;
	// 一番優先度の高いカメラを取得

	auto it = cameraView.begin();
	auto end = cameraView.end();

	if (it == end) return;

	for (; it != end; ++it)
	{

		ComponentHandle<Camera> itCamera = a_chunk.GetComponent<Camera>(*it);

		if (!useCamera.IsValid() || (itCamera.Look().cameraPriority > useCamera.Look().cameraPriority))
		{
			useCamera = itCamera;
			cameraPosition = a_chunk.GetComponent<Position>(*it);
		}
	}

	if (!useCamera.IsValid() || !cameraPosition.IsValid()) return;

	DrawMatrix::CreateViewMatrix(wvp[1], float3{ cameraPosition->x, cameraPosition->y, cameraPosition->z }, useCamera->lookPosition, useCamera->upVector);
	DrawMatrix::CreateProjectionMatrix(wvp[2], useCamera->fovy, useCamera->aspect, useCamera->nearCrip, useCamera->farCrip);

	Geometory::SetView(wvp[1]);
	Geometory::SetProjection(wvp[2]);



	ComponentView obbView = a_chunk.GetView<ComponentTypes<OBBCollider>>();

	DirectX::XMFLOAT4 hitColor(1.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMFLOAT4 notHitColor(0.0f, 0.0f, 1.0f, 1.0f);

	for (auto it : obbView)
	{
		const ComponentHandle<OBBCollider> collider = a_chunk.GetComponent<OBBCollider>(it);

		float3 vertices[8];

		for (int x = 0; x < 2; x++)
		{
			for (int y = 0; y < 2; y++)
			{
				for (int z = 0; z < 2; z++)
				{
					float directionX;
					float directionY;
					float directionZ;

					directionX = (x == 0 ? 1.0f : -1.0f);
					directionY = (y == 0 ? 1.0f : -1.0f);
					directionZ = (z == 0 ? 1.0f : -1.0f);

					vertices[z + y * 2 + x * 4] =
						collider.Look().center
						+ collider.Look().edges[0] * directionX / 2.0f
						+ collider.Look().edges[1] * directionY / 2.0f
						+ collider.Look().edges[2] * directionZ / 2.0f;
				}
			}
		}

		static const int edges[12][2] =
		{
			{0,1}, {0,2}, {0,4},
			{1,3}, {1,5},
			{2,3}, {2,6},
			{3,7},
			{4,5}, {4,6},
			{5,7},
			{6,7}
		};


		for (int i = 0; i < 12; i++)
		{
			int a = edges[i][0];
			int b = edges[i][1];

			DirectX::XMFLOAT3 dxA(vertices[a].x, vertices[a].y, vertices[a].z);
			DirectX::XMFLOAT3 dxB(vertices[b].x, vertices[b].y, vertices[b].z);

			const ComponentHandle<HitInfomation> info = a_chunk.GetComponent<HitInfomation>(it);

			DirectX::XMFLOAT4 color = info.IsValid() ? hitColor : notHitColor;

			Geometory::AddLine(dxA, dxB, color);
		}


	}
}

void SectorDraw(Chunk& a_chunk, const SystemContext& a_context, ComponentView cameraView)
{
	// !!!New!!!
	DirectX::XMFLOAT4X4 wvp[3];
	ComponentHandle<Camera> useCamera;
	ComponentHandle<Position> cameraPosition;

	auto it = cameraView.begin();
	auto end = cameraView.end();
	if (it == end) return;

	for (; it != end; ++it)
	{
		ComponentHandle<Camera> itCamera = a_chunk.GetComponent<Camera>(*it);
		if (!useCamera.IsValid() || (itCamera.Look().cameraPriority > useCamera.Look().cameraPriority))
		{
			useCamera = itCamera;
			cameraPosition = a_chunk.GetComponent<Position>(*it);
		}
	}

	if (!useCamera.IsValid() || !cameraPosition.IsValid()) return;

	DrawMatrix::CreateViewMatrix(wvp[1], float3{ cameraPosition->x, cameraPosition->y, cameraPosition->z }, useCamera->lookPosition, useCamera->upVector);
	DrawMatrix::CreateProjectionMatrix(wvp[2], useCamera->fovy, useCamera->aspect, useCamera->nearCrip, useCamera->farCrip);
	Geometory::SetView(wvp[1]);
	Geometory::SetProjection(wvp[2]);

	DirectX::XMFLOAT4 attackColor(1.0f, 0.4f, 0.0f, 1.0f);
	DirectX::XMFLOAT4 waitColor(0.0f, 0.4f, 1.0f, 1.0f);
	constexpr int kArcSegments = 16;

	// 描画用ラムダ
	auto drawSectorLines = [&](
		float minLength,
		float maxLength,
		float angle,
		float maxHeight,
		float maxLowness,
		float posX,
		float posY,
		float posZ,
		float yawDeg,
		const DirectX::XMFLOAT4& drawColor)
	{
		float yawRad = yawDeg * RAD;
		float halfAngleRad = angle * 0.5f * RAD;
		float topY = posY + maxHeight;
		float bottomY = posY - maxLowness;

		auto makePoint = [&](float length, float angleOffsetRad, float y) -> DirectX::XMFLOAT3
		{
			float dirAngle = yawRad + angleOffsetRad;
			return DirectX::XMFLOAT3(
				posX + sinf(dirAngle) * length,
				y,
				posZ + cosf(dirAngle) * length);
		};

		auto drawSectorAtY = [&](float y)
		{
			DirectX::XMFLOAT3 leftInner = makePoint(minLength, -halfAngleRad, y);
			DirectX::XMFLOAT3 leftOuter = makePoint(maxLength, -halfAngleRad, y);
			DirectX::XMFLOAT3 rightInner = makePoint(minLength, halfAngleRad, y);
			DirectX::XMFLOAT3 rightOuter = makePoint(maxLength, halfAngleRad, y);
			Geometory::AddLine(leftInner, leftOuter, drawColor);
			Geometory::AddLine(rightInner, rightOuter, drawColor);

			for (int i = 0; i < kArcSegments; ++i)
			{
				float t0 = static_cast<float>(i) / static_cast<float>(kArcSegments);
				float t1 = static_cast<float>(i + 1) / static_cast<float>(kArcSegments);
				float a0 = -halfAngleRad + (halfAngleRad * 2.0f) * t0;
				float a1 = -halfAngleRad + (halfAngleRad * 2.0f) * t1;

				Geometory::AddLine(makePoint(maxLength, a0, y), makePoint(maxLength, a1, y), drawColor);
				if (minLength > 0.0f)
				{
					Geometory::AddLine(makePoint(minLength, a0, y), makePoint(minLength, a1, y), drawColor);
				}
			}
		};

		drawSectorAtY(topY);
		drawSectorAtY(bottomY);

		Geometory::AddLine(makePoint(maxLength, -halfAngleRad, topY), makePoint(maxLength, -halfAngleRad, bottomY), drawColor);
		Geometory::AddLine(makePoint(maxLength, halfAngleRad, topY), makePoint(maxLength, halfAngleRad, bottomY), drawColor);
		if (minLength > 0.0f)
		{
			Geometory::AddLine(makePoint(minLength, -halfAngleRad, topY), makePoint(minLength, -halfAngleRad, bottomY), drawColor);
			Geometory::AddLine(makePoint(minLength, halfAngleRad, topY), makePoint(minLength, halfAngleRad, bottomY), drawColor);
		}
		else
		{
			DirectX::XMFLOAT3 originTop(posX, topY, posZ);
			DirectX::XMFLOAT3 originBottom(posX, bottomY, posZ);
			Geometory::AddLine(originTop, originBottom, drawColor);
		}
	};

	// 当たり判定描画
	ComponentView sectorView = a_chunk.GetView<ComponentTypes<SectorHitJudge, Position, Rotation>>();
	for (auto sectorIt : sectorView)
	{
		const ComponentHandle<SectorHitJudge> sector = a_chunk.GetComponent<SectorHitJudge>(sectorIt);
		const ComponentHandle<Position> pos = a_chunk.GetComponent<Position>(sectorIt);
		const ComponentHandle<Rotation> rot = a_chunk.GetComponent<Rotation>(sectorIt);

		drawSectorLines(
			sector.Look().minLength,
			sector.Look().maxLength,
			sector.Look().angle,
			sector.Look().maxHeight,
			sector.Look().maxLowness,
			pos.Look().x,
			pos.Look().y,
			pos.Look().z,
			rot.Look().yaw,
			attackColor);
	}

	// 判定表示を描画
	ComponentView telegraphView = a_chunk.GetView<ComponentTypes<AttackTelegraph, Position, Rotation>>();
	for (auto telegraphIt : telegraphView)
	{
		const ComponentHandle<AttackTelegraph> telegraph = a_chunk.GetComponent<AttackTelegraph>(telegraphIt);
		const ComponentHandle<Position> pos = a_chunk.GetComponent<Position>(telegraphIt);
		const ComponentHandle<Rotation> rot = a_chunk.GetComponent<Rotation>(telegraphIt);

		drawSectorLines(
			telegraph.Look().minLength,
			telegraph.Look().maxLength,
			telegraph.Look().angle,
			telegraph.Look().maxHeight,
			telegraph.Look().maxLowness,
			pos.Look().x,
			pos.Look().y,
			pos.Look().z,
			rot.Look().yaw,
			waitColor);
	}
}
