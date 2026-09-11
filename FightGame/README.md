# FightGame

「間合い」をコンセプトにした 3D アクションバトルゲームです。
DirectX 11 と自作 ECS（Entity Component System）基盤の上に、扇形の攻撃判定・ユーティリティ AI・
シーン（World）ごとの System 実行順という構成で作っています。

- 言語 / 環境: C++17 / Visual Studio / Windows x64
- 描画: DirectX 11（自作描画基盤 + 自作 HLSL シェーダ）
- 自作コード規模: 約 214 ファイル / 約 26,600 行（imGui・assimp・DirectXTex 等の外部ライブラリを除く）

---

## 1. このゲームでやっていること

### コンセプト: 有利な間合いを探して、維持する

殴り合いの上手さではなく、**どの距離に立つか**で勝敗が決まる作りを狙っています。

- プレイヤーの攻撃は「近距離（射程 1.7 / 扇 160°）」と「遠距離（射程 6.2 / 扇 60°）」の 2 種類
- 敵の攻撃はそれぞれ有効な距離帯が違う（例: 射程 2.5 の全周攻撃、1.3〜3.7 の中距離、3.5〜7.5 の狙撃）
- つまり「敵の攻撃が届かないが、自分の攻撃は届く帯」が存在する。そこに立てれば一方的に殴れる
- 逆に敵の得意距離に入ると、こちらは選択肢を失って押し切られる

攻撃データは `Assets/Data/AttackData.json` に外出しされており、射程・扇の角度・ダメージ・
前隙（StartupTime）・後隙（WaitTime）・持続（LifeTime）を JSON で調整できます。

### 対抗アクション

敵の攻撃に対して、耐えるか避けるかを選びます。

- **ガード**: 押している間ダメージを軽減する。防ぎきれないので耐久には限界がある
- **ジャンプ**: 上方向に逃げて判定を外す。成功すればノーダメージだが、タイミングはシビア

どちらも「攻撃を食らう位置」だけでなく「ギリギリ当たらない位置」でも成立するため、
敵の攻撃範囲とタイミングを見切れば、リスクを負わずに立ち回れます。

### 敵

敵は毎フレーム場当たりに動くのではなく、**位置関係の目標（Stance）を持って**行動します。

- 不利脱出 / 有利維持 / ニュートラル様子見 の 3 つの目標を、距離・被弾数・継続時間からスコアリングして選ぶ
- 選んだ目標の下で、接近・離間・技 0・技 1・待機 などの手段を Behavior Tree で実行する
- 同じ技を連打していないか（`sameAttackStreak`）などの「証拠」を溜め、閾値を超えたら目標を再評価する

### ロックオン

一定距離内の敵を自動で捕捉し、解除半径の外に出るまで敵の方向（yaw のみ）を向き続けます。
向きを合わせる操作コストを外すことで、プレイヤーの注意を距離の選択に集めるのが目的です。
捕捉 7.0 / 解除 7.8 のヒステリシスを持たせ、遠距離攻撃の射程内でロックが切れないようにしています。

### 収録シーン

Startup / Load / Title / Tutorial / Proto / Result / Clear の各 World が繋がっており、
間合い格闘の本線は **Proto** です。ほかに開発用の Test と、ステージ編集用の CreateGame があります。

---

## 2. 操作方法

### 基本操作

| 操作 | キーボード / マウス | ゲームパッド（XInput） |
| --- | --- | --- |
| 移動 | `W` `A` `S` `D` | 左スティック |
| カメラ回転 | マウス移動 | 右スティック |
| 近距離攻撃 | 右クリック | `B` |
| 遠距離攻撃 | 左クリック | `A` |
| ジャンプ | `E` | `RB` |
| ガード | `Q`（押している間） | `LB`（押している間） |
| ロックオン対象の切替 | マウスを大きく動かす | 右スティック |

- 移動入力はカメラの yaw でワールド方向に変換されるため、「下がる」はカメラ基準の後退になります
- 左スティックとキーボードは入力量の大きい方が自動採用されます（同時接続でも競合しません）
- 近距離攻撃と遠距離攻撃を同時に押した場合は近距離（右クリック / `B`）が優先されます
- ロックオンに専用ボタンはありません。捕捉と解除は距離で自動、対象の切替だけが手動です

### シーン操作・デバッグ

| 操作 | 割り当て | 内容 |
| --- | --- | --- |
| 決定（タイトル） | `Space` / `A` | シーン開始 |
| リトライ | 左クリック / `A` | ゲームオーバー後の再開 |
| チュートリアルスキップ | `Enter` | チュートリアル飛ばし |
| ImGui 表示切替 | `F2` | デバッグ UI の開閉 |
| デバッグシーンへ | `F3` | デバッグ用シーン遷移 |
| デバッグコンソール | `F1` | コンソール表示 |
| 終了 | `Esc` / `START` | ゲーム終了 |

`F1` / `F2` / `F3` は Debug ビルドでのみ有効です。
デバッグシーンではカメラを `WASD` ＋ マウスで自由に動かせます。

---

## 3. 技術的アピール

### 3-1. 自作 ECS 基盤（`include/ECS/Core`）

外部の ECS ライブラリを使わず、Entity / Component / System / Chunk / World を自作しています。

- **型ごとの密なストレージ**: `ComponentStorage<T>` が `unordered_map<Entity, index>` +
  `vector<ComponentPack>` を持ち、削除は swap-and-pop で O(1)。Component 実体は連続配置されます
- **型リストによるクエリ**: `chunk.GetView<ComponentTypes<Position, Velocity>>()` のように
  必要な Component をテンプレート引数で渡すと、その全てを持つ Entity だけを列挙する View が得られます。
  第 2 引数に除外用の型リストを渡せば「A を持つが B を持たない」も書けます
- **読み書きを型で区別する `ComponentHandle`**: `handle->member`（書き込み）と
  `handle.Look().member`（読み取り専用）を分け、書き込み側だけが変更フラグを立てる設計にしています
- **型消去したストレージ管理**: `StorageManager` が `TypeID → IComponentStorage` で保持し、
  初めてその型を `AddComponent` した時点でストレージを動的に生成します
- **Entity は id + generation**: 削除時に generation を進めることで、
  ID を再利用しても古い参照が有効化しないようにしています
- **System は状態を持たない自由関数**: `void FooSystem(Chunk&, const SystemContext&)` の形に統一。
  実行順の責務は System 側に持たせず、World の呼び出し順だけが唯一の真実になります
- **World が実行順を明示列挙**: 初回だけ必要な処理（Pose 確定など）を `InitChunk` に、
  毎フレームの処理を `UpdateChunk` に分け、シーンごとにパイプラインを組み替えられます
- 現在 147 個の Component 定義、95 個の System 関数、24 個の System ヘッダで構成されています

### 3-2. 座標更新を「結果コンポーネント」に集約した移動パイプライン

複数の移動処理が同じフレームで `Position` を直接書き換えると、上書き合戦や打ち消しが起きます。
そこで各移動 System は座標を直接触らず、**結果を書き込むだけ**にしました。

| コンポーネント | 役割 | 書き込む側 |
| --- | --- | --- |
| `FixedResult` | 絶対座標・絶対角度の決定 | 追従、注視、レール、跳躍 |
| `MotionResult` | そのフレームの移動・回転の差分（ワープ指定も可） | 入力移動、前進、旋回 |
| `EphemeralResult` | 1 フレーム限りのオフセット（揺れなど） | 画面揺れ、振動 |
| `MotionTransform` | `MotionResult` の差分をフレームを跨いで累積 | Pose 系 System |

これらを `PoseSystem` が 1 か所で合成し、`TransformSystem` が `Position` / `Rotation` へ反映、
`ResetSystem` が結果を掃除します。カメラや追従のように「後で確定させたい」対象は
`LatePoseSystem` に回すことで、同一フレーム内の依存順も表現できます。

### 3-3. Component の JSON シリアライズ（型情報の手書きリフレクション）

`ComponentField.h` に Component ごとの `ApplyToFields(component, valueFunc, entityFunc)` を用意し、
フィールド名と値を訪問できるようにしています。この訪問関数ひとつで、

- `nlohmann::json` への保存（`TypeId` / `Version` / `TypeName` + `data` 階層）
- JSON からの復元（`TypeID` をキーにした生成関数レジストリ経由で `std::any` として受け取る）

の双方向を実装しています。**Entity 参照はそのまま保存すると壊れる**ため、
保存時に Entity 配列内のインデックスへ変換し、読み込み時に新しい Entity へ張り直しています。
Chunk 単位の保存・読み込み（`OutJsonComponents` / `LoadJsonComponent`）まで通っています。

### 3-4. 三層構造の敵 AI（Utility + Behavior Tree）

`include/AI` に 23 ファイルで自作の Behavior Tree と Utility 評価を実装しています。

- **層の分離**: Role（性質＝カーブ係数）→ Stance（目標）→ Action（手段）の 3 段に分け、
  変化する速さを揃えないようにしています（性質はゆっくり、目標はときどき、手段は毎 Tick）
- **Utility による目標選択**: `StanceNode` が距離・被弾数・継続時間からスコアを出して argmax を取ります。
  継続ボーナスは「現在の Stance のスコアにだけ」掛ける（全候補に掛けると argmax で打ち消されるため）
- **性格を 0〜1 のノブで表現**: `RoleCurveKnobs`（距離のこだわり、遠さの好み、被弾感度、目標の粘り）を
  内部の係数へマッピングする層を挟み、敵の性格差を係数調整だけで作れるようにしています
- **データの置き場を責務で分割**: `AIBlackboard`（入力）/ `AIMind`（思考の状態と記憶）/
  `AIContext`（木の進行度）/ `AIResult`（今 Tick の出力）。System 側は `AIResult` だけを見ます
- ノード群は `Selector` / `Sequencer` / `ReactiveSelector` / `Decorator` / `ActionNode` などを自作
- **AI の思考は 3 フレームに 1 回**に間引き、移動や当たり判定はフレーム毎に回しています。
  プレイヤー入力も敵 AI も同じ `MoveInputResult` に集約されるため、
  入力元（デバイス / AI）を差し替えるだけで同じ移動・攻撃経路を共有できます

### 3-5. 描画

- **扇形メッシュの動的生成**: 攻撃範囲の扇を専用シェーダで描画します（`Geometory::DrawSector`）。
  内半径・外半径・角度・高さから頂点を生成し、進行度に応じて色を変えられるため、
  「最初は薄く、攻撃が近づくと色が付く」予兆表現に使えます
- **攻撃データからの自動登録**: `AttackData.json` のキーを走査して扇メッシュを一括登録するため、
  攻撃を JSON に足せば描画側の追加作業は不要です
- **シェーダは実行時コンパイル**: HLSL をソース内に持ち `D3DCompile` で起動時に生成します。
  Lambert / Specular / Toon / Fog / Ghost / アルファブレンド、ボーン対応の頂点シェーダを用意
- **当たり判定**: OBB の生成・判定・押し戻しに加え、レイと扇形の判定を自作
- **アニメーション**: Assimp のスケルタルアニメ（最大 200 ボーン）とモーフターゲットに対応
- **非同期ロード**: モデル・テクスチャ・エフェクト・サウンドをワーカースレッドと
  スレッドセーフキューで読み込み、ロード画面と並行させています
- モデル読み込みに Assimp、テクスチャに DirectXTex、エフェクトに Effekseer、
  文字描画に DirectWrite、音声に XAudio2 を使用

### 3-6. デバッグ環境 / 簡易ステージエディタ

ImGui 上で ECS の中身をそのまま編集できるようにしています（`ImGuiSystem` / `Chunk::ImGuiInPut`）。

- Entity の新規作成・削除、Component の追加・削除、名前や種別での検索
- 選択した Entity の全 Component を、3-3 の `ApplyToFields` を再利用してフィールド単位で編集
- Chunk 全体を JSON に保存 / 任意のキーから読み込み（デバッグ専用 Entity は保存対象外）
- デバッグカメラ、デバッグ用シーン、コンソール出力の切り替え
- Component 定義に `kTypeId` / `kTypeName` / `kVersion` を持たせ、ImGui 表示とシリアライズで共用。
  TypeID / TypeName の重複も ImGui 上で検出できます

つまり**編集 UI とセーブデータが同じ訪問関数 1 本から生成されている**のが、この基盤の要点です。
Component を 1 つ足せば、保存・復元・ImGui 編集が同時に付いてきます。

### 3-7. 使用ライブラリ

Assimp（モデル）、DirectXTex（テクスチャ）、Effekseer（エフェクト）、Dear ImGui（デバッグ UI）、
nlohmann/json（シリアライズ）、Direct2D / DirectWrite（文字）、XAudio2（音声）、XInput（パッド）、zlib。
描画・入力・ECS・AI・シリアライズの層は自作です。

---

## 4. ビルド

1. `FightGame.sln` を Visual Studio で開く
2. 構成を `Debug` または `Release`、プラットフォームを `x64` にする
3. ビルドして実行（C++17 / Windows 10 SDK）

外部ライブラリは `Lib/` 配下の静的ライブラリをリンクしています。

---

## 5. ディレクトリ構成

| パス | 内容 |
| --- | --- |
| `include/ECS/Core` | ECS 基盤（Chunk、ストレージ、View、シリアライズ） |
| `include/ECS/Component` | Component 定義（`Components.h`） |
| `include/ECS/System` | System 宣言 |
| `include/ECS/World` | シーン（World）定義と System 実行順 |
| `include/AI` | Behavior Tree / Utility 評価 / 各ノード |
| `Source/Drawer` | 描画基盤（`Geometory` など） |
| `Assets/Data` | 攻撃データなどの JSON |
| `docs/` | 仕様・設計メモ（意図のドキュメント） |

設計意図の詳細は [`docs/README.md`](docs/README.md) を目次として、
[`docs/game-overview.md`](docs/game-overview.md)、[`docs/ai-behavior.md`](docs/ai-behavior.md)、
[`docs/lock-on.md`](docs/lock-on.md) にまとめています。

---

## 6. 現状と未実装

制作途中のため、以下は未完了です（詳細は [`docs/implementation-tasks.md`](docs/implementation-tasks.md)）。

- 敵攻撃の予兆表示（透明 → 攻撃開始で色が付く）は描画側の仕組みのみ用意済み
- 対抗アクション成功時の強化（役割差を付ける段階）は未実装
- 勝利条件・ステージ遷移・リザルトまでのゲームループ接続
- ヒットエフェクト、アニメーションの Model からの分離
- `ComponentStorage` の変更検知（version / changedEntities）は枠のみで、現在は未使用
- レールシューター系の `GameWorld` は更新処理が空のスタブ（本線は Proto 側）

---

## 7. クレジット

使用したモデル・音源のクレジットは [`Credit.txt`](Credit.txt) を参照してください。
