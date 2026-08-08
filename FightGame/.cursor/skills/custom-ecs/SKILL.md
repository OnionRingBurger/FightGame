---
name: custom-ecs
description: 自作ECS(共通基盤)の仕様とSystemの書き方。Chunk/Entity/ComponentView/ComponentHandle/SystemContextを使うコードの読み書き、Systemの新規作成、移動系(TransformSystem・Pose・MotionResult等)の変更を行うときに使う。ECS、System、Component、Entity、移動処理、座標更新に関する依頼で参照する。
---

# 自作ECS

自作のECS共通基盤。ヘッダは `include/ECS/` 配下(`Core` / `Component` / `System` / `World`)、実装は `Source/ECS/` 配下に同じ構成で配置される。

## 基本構造

- **Entity**: ID。`Chunk::CreateNewEntity(components...)` で生成する
  - 無効(空)のEntityは `Components.h` に `constexpr Entity kInvalidEntity`(-1, -1)として定義済み。無効値が必要なときは必ずこれを使い、新たに空のEntity定数や `{-1, -1}` のような即値を作らない
- **Component**: `include/ECS/Component/Components.h` の `namespace Component` 内に定義された純粋なデータ構造体。各structは `kTypeId` / `kTypeName` / `kVersion` を持つ
  - **新規 Component の追加位置**: 関連タグの隣や途中には挿まない。`namespace Component` 内の**既存定義の最後尾**（名前空間を閉じる `}` の直前）に追加する
  - `kTypeId` は既存の最大値の次を使う。途中の空き番号を埋めない（末尾追加と揃える）
- **System**: `void FooSystem(Chunk& a_chunk, const SystemContext& a_context)` 形式の自由関数。状態を持たない
- **Chunk**: EntityとComponentの管理者。SystemはChunk経由でComponentにアクセスする
- **World**(`include/ECS/World/`): 各シーン(GameWorld等)がUpdate内でSystemを呼び出し順に実行する。**Systemの実行順はWorldの呼び出し順がすべて**

## Systemの書き方

```cpp
using namespace Component;

void FooSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	// 必要なComponentを全て持つEntityを列挙するView
	ComponentView view = a_chunk.GetView<ComponentTypes<Position, Velocity>>();

	for (auto it : view)  // it は Entity
	{
		ComponentHandle<Position> pos = a_chunk.GetComponent<Position>(it);
		const ComponentHandle<Velocity> vel = a_chunk.GetComponent<Velocity>(it);

		pos->x += vel.Look().x * a_context.deltaTime;
	}
}
```

- **ComponentHandle の使い分けが最重要**:
  - `handle->member` = 書き込み用。`operator->` は変更フラグ(version)を立てるので、**読み取りだけなら使わない**
  - `handle.Look().member` = 読み取り専用。読むだけなら必ずこちら
  - Viewに含まれないEntity(ターゲット等)から取得した場合は `IsValid()` を確認する
- 除外条件付きView: `GetView<ComponentTypes<A>, ComponentTypes<B>>()`(Bを持つEntityを除外)
- `SystemContext` は `input`(入力)、`deltaTime`、`effectStepTime`、`modelCache` / `uiCache` 等を持つ
- 角度は**度数法**で保持し、行列計算時に `RAD` を掛けてラジアン化する。`Rotation` は pitch/yaw/roll

## 移動系パイプライン(TransformSystem周り)

座標・回転は直接 `Position` / `Rotation` を書き換えず、**Resultコンポーネントに書き込み、後段のPoseSystem系が合成する**。

### 3種のResult + 蓄積用Component

| Component | 役割 | 書き込む例 |
|---|---|---|
| `FixedResult` (newPos/newRot) | 絶対座標・絶対角度の決定 | Follow、Leap、Look、Rail |
| `MotionResult` (posOffset/rotOffset, isWarp/warpPos) | そのフレームの移動・回転の差分。ワープ指定も可 | InputMove、InputRotato、MoveForward |
| `EphemeralResult` (posOffset/rotOffset) | 1フレーム限りのオフセット(揺れ等) | Shake、Shaking |
| `MotionTransform` (motionPos/motionRot) | MotionResultの差分を**フレームを跨いで蓄積**する | PoseSystem系が更新するため基本的に直接書きこまない |

### 合成の流れ(Source/ECS/System/TransformSystem.cpp)

1. 各移動SystemがResultに書き込む
2. `PoseSystem`: `Pose.pos = FixedResult.newPos + MotionTransform.motionPos(+= MotionResult.posOffset) + EphemeralResult.posOffset`。回転も同様(AngleLimitComponentによるクランプあり)。`MotionResult.isWarp` 時は warpPos との差分を motionPos に反映
3. `LatePoseSystem`: カメラ・追従系など後で確定させたいEntityのPoseを同じ式で決定
4. `TransformSystem`: 全Entityの `Pose` を `Position` / `Rotation` に反映(ApplyPoseToTransform)
5. `ResetSystem`: Resultをリセット

早い/遅い の振り分けは `PosePosState` / `PoseRotState` のenum値で行う(RAIL・LEAP・NONEは先、CAMERA・FOLLOW・LOOK等は遅延)。

### Worldでの実行順(GameWorld::UpdateChunk)

```text
VelocitySystem → Collider系
→ 移動系System群(InputMove, InputRotato, Flip, Look, MoveForward, Leap, Shaking, TrackingWarp, Rail, Fly, Shake)
→ PoseSystem
→ 遅延系(FollowTransformSystem, CameraMoveSystem) → LatePoseSystem
→ TransformSystem
→ Transform結果を使うSystem(Ray, Laser等) → ResetSystem
```

## 新しいSystemを追加するとき

1. `include/ECS/System/FooSystem.h` と `Source/ECS/System/FooSystem.cpp` を作成(cpp-file-pairスキルの規約に従う)
2. 移動に関わるSystemなら `Position` を直接書き換えず、上記のResultコンポーネントに書き込む
3. WorldのUpdate内の適切な位置(PoseSystemより前か後か)に呼び出しを追加する
4. プロジェクト固有の追加手順(System.hへのinclude追加等)はプロジェクトのRulesに従う

## CreateNewEntity / Entity の Component 列

- 既存 Entity の Component 追加・削除・差し替えは、ユーザーが Component 名を明示したときだけ行う
- 「AI登録」「World配線」「動くようにする」だけでは Component 列を変えない。不足があれば列挙して確認する
- プロジェクト Rules の `edit-approval`（Entity / CreateNewEntity 条項）に従う

## 新しい Component を追加するとき

1. 定義は必ず [`include/ECS/Component/Components.h`](include/ECS/Component/Components.h) の `namespace Component` **末尾**に書く（類似 Component の隣への挿入はしない）
2. `kTypeId` はファイル内の既存最大 TypeID の次番号を割り当てる
3. `Components.h` は要承認ファイル。編集前に対象ファイル名と追加 Component 名を出して承認を得る
4. 新規まとまりにはユーザールールどおり `!!!New!!!` を残す。既存コメントは消さない
