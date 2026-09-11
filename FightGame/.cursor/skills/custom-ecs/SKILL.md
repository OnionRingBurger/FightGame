---
name: custom-ecs
description: 自作ECS(共通基盤)の仕様とSystemの書き方�?Chunk/Entity/ComponentView/ComponentHandle/SystemContextを使�?コード�?�読み書き、Systemの新規作�?�、移動系(TransformSystem・Pose・MotionResultなど)の変更を行うときに使�?�?ECS、System、Component、Entity、移動�?�回転、座標更新に関する依�?�で参�?�する�?
---

# 自作ECS

自作�?�ECS共通基盤。�?�ッダは `include/ECS/` 配�?(`Core` / `Component` / `System` / `World`)、実�?は `Source/ECS/` 配下に同じ構�?�で配置される�?

## 基本構�?

- **Entity**: ID。`Chunk::CreateNewEntity(components...)` で生�?�す�?
  - 無効(空)のEntityは `Components.h` に `constexpr Entity kInvalidEntity`(-1, -1)として定義済み。無効値が�?要なとき�?��?ずこれを使�?、新たに空のEntity定数�? `{-1, -1}` のような即値を作らな�?
- **Component**: `include/ECS/Component/Components.h` の `namespace Component` �?に定義された純粋な�?ータ構�?体。各structは `kTypeId` / `kTypeName` / `kVersion` を持つ
  - **新�? Component の追�?位置**: 関連タグの隣�?途中には挿まな�?。`namespace Component` �?の**既存定義の最後尾**?��名前空間を閉じ�? `}` の直前）に追�?する
  - `kTypeId` は既存�?�最大値の次を使�?。途中の空き番号を埋めな�??��末尾追�?と�?える?�?
- **System**: `void FooSystem(Chunk& a_chunk, const SystemContext& a_context)` 形式�?�自由関数。状態を持たな�?
- **Chunk**: EntityとComponentの管�?単位。SystemはChunk経由でComponentにアクセスする
- **World**(`include/ECS/World/`): �?シーン(GameWorldなど)がUpdate�?でSystemを呼び出し�??に実行する�?**Systemの実行�??はWorldの呼び出し�??がすべて**

## Systemの書き方

```cpp
using namespace Component;

void FooSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	// �?要なComponentをすべて持つEntityを�?�挙するView
	ComponentView view = a_chunk.GetView<ComponentTypes<Position, Velocity>>();

	for (auto it : view)  // it は Entity
	{
		ComponentHandle<Position> pos = a_chunk.GetComponent<Position>(it);
		const ComponentHandle<Velocity> vel = a_chunk.GetComponent<Velocity>(it);

		pos->x += vel.Look().x * a_context.deltaTime;
	}
}
```

- **ComponentHandle の使�?�?けが最重�?**:
  - `handle->member` = 書き込み用。`operator->` は変更フラグ(version)を立てる�?�で�?**読み取り�?けなら使わな�?**
  - `handle.Look().member` = 読み取り専用。読む�?けなら�?ずこち�?
  - Viewに含まれないEntity(ターゲ�?ト�?)から取得した�?�合�?� `IsValid()` を確認す�?
- 除外条件付きView: `GetView<ComponentTypes<A>, ComponentTypes<B>>()`(Bを持つEntityを除�?)
- `SystemContext` は `input`(入�?)、`deltaTime`、`effectStepTime`、`modelCache` / `uiCache` 等を持つ
- 角度は**度数�?**で保持し、行�?�計算時に `RAD` を掛けてラジアン化する。`Rotation` は pitch/yaw/roll

## 移動系パイプライン(TransformSystem周辺)

座標�?�回転は直接 `Position` / `Rotation` を書き換えず�?**Resultコンポ�?�ネントに書き込み、後段のPoseSystem系が合成す�?**�?

### 3種のResult + 累積用Component

| Component | 役割 | 書き込む側 |
|---|---|---|
| `FixedResult` (newPos/newRot) | 絶対座標�?�絶対角度の決�? | Follow、Leap、Look、Rail |
| `MotionResult` (posOffset/rotOffset, isWarp/warpPos) | そ�?�フレー�?の移動�?�回転の差�?。ワープ指定も可 | InputMove、InputRotato、MoveForward |
| `EphemeralResult` (posOffset/rotOffset) | 1フレー�?限りのオフセ�?�?(揺れ�?) | Shake、Shaking |
| `MotionTransform` (motionPos/motionRot) | MotionResultの差�?�?**フレー�?を跨�?で�?�?**する | PoseSystem系が更新するため基本�?に直接書きこまな�? |

### 合�?��?�流れ(Source/ECS/System/TransformSystem.cpp)

1. �?移動SystemがResultに書き込む
2. `PoseSystem`: `Pose.pos = FixedResult.newPos + MotionTransform.motionPos(+= MotionResult.posOffset) + EphemeralResult.posOffset`。回転も同�?(AngleLimitComponentによるクランプあ�?)。`MotionResult.isWarp` 時�?� warpPos との差�?�? motionPos に反映
3. `LatePoseSystem`: カメラ・追従系など後で確定させたいEntityのPoseを同じ式で決�?
4. `TransformSystem`: 全Entityの `Pose` �? `Position` / `Rotation` に反映(ApplyPoseToTransform)
5. `ResetSystem`: Resultをリセ�?�?

早�?/�?延 の振り�??け�?� `PosePosState` / `PoseRotState` のenum値で行う(RAIL・LEAP・NONEは先、CAMERA・FOLLOW・LOOK等�?��?延)�?

### Worldでの実行�??(GameWorld::UpdateChunk)

```text
VelocitySystem �? Collider系
�? 移動系System群(InputMove, InputRotato, Flip, Look, MoveForward, Leap, Shaking, TrackingWarp, Rail, Fly, Shake)
�? PoseSystem
�? �?延系(FollowTransformSystem, CameraMoveSystem) �? LatePoseSystem
�? TransformSystem
�? Transform結果を使�?System(Ray, Laser�?) �? ResetSystem
```

## 新しいSystemを追�?すると�?

1. `include/ECS/System/FooSystem.h` と `Source/ECS/System/FooSystem.cpp` を作�??(cpp-file-pairスキルの規�?に従う)
2. 移動に関わるSystemな�? `Position` を直接書き換えず、上記�?�Resultコンポ�?�ネントに書き込む
3. WorldのUpdate�?の適�?な位置(PoseSystemより前か後か)に呼び出しを追�?する
4. プロジェクト固有�?�追�?手�??(System.hへのinclude追�?など)はプロジェクト�?�Rulesに従う

## CreateNewEntity / Entity の Component �?

- 既�? Entity の Component 追�?・削除・差し替え�?�、ユーザー�? Component 名を明示したときだけ行う
- 「AI登録」「World配線」「動くよ�?にする」だけでは Component 列を変えな�?。不足があれ�?�列挙して確認す�?
- プロジェク�? Rules の `edit-approval`?�?Entity / CreateNewEntity 条�??��に従う

## 新しい Component を追�?すると�?

1. 定義は�?�? [`include/ECS/Component/Components.h`](include/ECS/Component/Components.h) の `namespace Component` **末尾**に書く（類似 Component の隣への挿入はしな�??�?
2. `kTypeId` はファイル�?の既存最大 TypeID の次番号を割り当て�?
3. `Components.h` は要承認ファイル。編�?前に対象ファイル名と追�? Component 名を出して承認を得る
4. コメント�?�プロジェク�? Rules `code-comments` に従う。`!!!New!!!` は**単独�?**のみ。説明文を同一行にも次行にも足さな�?。既存コメント�?�消さな�?
