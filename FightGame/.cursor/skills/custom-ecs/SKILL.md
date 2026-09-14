---
name: custom-ecs
description: 自作ECS(共通基盤)の仕様とSystemの書き方。Chunk/Entity/ComponentView/ComponentHandle/SystemContextを使うコードの読み書き、Systemの新規作成、移動系(TransformSystem・Pose・MotionResultなど)の変更を行うときに使う。ECS、System、Component、Entity、移動・回転、座標更新に関する依頼で参照する。
---

# 自作ECS

自作のECS共通基盤。ヘッダは `include/ECS/` 配下（`Core` / `Component` / `System` / `World`）、実装は `Source/ECS/` 配下に同じ構成で配置される。

## 基本構造

- **Entity**: ID。`Chunk::CreateNewEntity(components...)` で生成する
  - 無効（空）の Entity は `Components.h` に `constexpr Entity kInvalidEntity`（-1, -1）として定義済み。無効値が必要なときは必ずこれを使い、新たに空の Entity 定数や `{-1, -1}` のような即値を作らない
- **Component**: `include/ECS/Component/Components.h` の `namespace Component` 内に定義された純粋なデータ構造体。各 struct は `kTypeId` / `kTypeName` / `kVersion` を持つ
  - **新規 Component の追加位置**: 関連タグの隣や途中には挿まない。`namespace Component` 内の**既存定義の最後尾**（名前空間を閉じる直前）に追加する
- **System**: `void FooSystem(Chunk& a_chunk, const SystemContext& a_context)` 形式の関数。ヘッダは `include/ECS/System/`、実装は `Source/ECS/System/`
- **Chunk**: Entity と Component の実体。System は Chunk を受け取り、ComponentView / ComponentHandle で走査・読み書きする
- **World**: `UpdateChunk` 内で System の呼び出し順を決める。World ごとに順序が異なることがある

## System の書き方

```cpp
void ExampleSystem(Chunk& a_chunk, const SystemContext& a_context)
{
    ComponentView view = a_chunk.GetView<ComponentTypes<Foo, Bar>>();

    for (auto it : view)
    {
        ComponentHandle<Foo> foo = a_chunk.GetComponent<Foo>(it);
        if (!foo.IsValid()) continue;

        ComponentHandle<Bar> bar = a_chunk.GetComponent<Bar>(it);
        bar->value = foo.Look().input * a_context.deltaTime;
    }
}
```

- 複数 Component を持つ Entity だけを対象にする場合は `GetView<ComponentTypes<...>>()` を使う
- 単一 Component の列挙は `GetHaveEntities()` 等、既存 System の先例に合わせる

## ComponentHandle の使い分け

定義は `include/ECS/Core/ComponentStorage.h` の `ComponentHandle<T>`。

| 操作 | 方法 | 用途 |
|------|------|------|
| 書き込み | `handle->member = ...` | 値を変更する（dirty フラグが立つ） |
| 読み取り | `handle.Look().member` | 参照のみ（変更しない） |
| 有効確認 | `handle.IsValid()` | 取得失敗時は必ず確認してから使う |

- 読み取りだけなのに `->` を使わない（不要な dirty を避ける）
- テンプレート経由の定義ジャンプは意図とずれることがある。実体は `ComponentStorage.h` を参照する

## 移動系パイプライン

座標・回転の更新は **Position / Rotation を直接書き換えない**。Result 系 Component にオフセットや確定値を書き、最後に `TransformSystem` が合成する。

| Component | 役割 |
|-----------|------|
| `FixedResult` | 確定位置・回転（`newPos` / `newRot` 等） |
| `MotionResult` | フレーム内の移動・回転オフセット（`posOffset` / `rotOffset`） |
| `EphemeralResult` | 一時的なオフセット（揺れ・演出など） |
| `MotionTransform` | 合成結果の中間（Transform 系が参照） |

典型的な流れ:

1. 各 Move / Pose 系 System が `MotionResult` や `FixedResult` に書く
2. `FollowTransformSystem` など遅延追従系が走る
3. `TransformSystem` が Result を合成し、`Position` / `Rotation` / `MotionTransform` を更新する

World の `UpdateChunk` では、おおむね **Physics → Pose 入力 → 追従・Look → TransformSystem → Transform 依存 System** の順。具体順序は対象 World（`ProtoWorld.cpp` 等）を Read して合わせる。

## 新しい System を追加するとき

1. `include/ECS/System/FooSystem.h` と `Source/ECS/System/FooSystem.cpp` をペアで作る（スキル `cpp-file-pair`）
2. `System.h` に `#include "FooSystem.h"` を追記（Rule: `system-file-generation.mdc`）
3. 対象 World の `UpdateChunk`（または `FixedUpdateChunk` 等）に呼び出しを追加 ? **World 編集は別途ユーザー承認が必要**
4. `register-source.ps1` で vcxproj 登録

## CreateNewEntity / Entity の Component 列

- `CreateNewEntity(...)` に渡す Component 列の追加・削除・差し替えは、ユーザーが **どの Component か名前で示したときだけ** 行う（Rule: `edit-approval`）
- 「動くようにする」「登録する」だけでは Component 列の拡張許可にならない
- マクロが `Components.h` 冒頭にあれば、個別列挙よりマクロを優先する（Rule: `component-macros.mdc`）

## 新しい Component を追加するとき

1. `Components.h` の `namespace Component` **末尾**に struct を追加 ? **`Components.h` 編集は別途ユーザー承認が必要**
2. シリアライズ等、既存の Component 追加先例（同種の Component）を Read して同じ経路で配線する
3. System から `GetComponent` / `GetView` で参照する
