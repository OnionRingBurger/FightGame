# ゲーム内容ドキュメント

このフォルダは、FightGame の**意図・仕様・設計メモ**を置く場所です。  
実行時の数値や実装の真実はソースコード側にあります。docs とコードがずれた場合は、どちらを正とするかを都度決めてください。

## 目次

| ファイル | 内容 |
| --- | --- |
| [game-overview.md](game-overview.md) | ゲーム概要、プレイヤー／敵の役割などの骨格 |
| [ai-behavior.md](ai-behavior.md) | 敵AI・間合い体験・Stance／リセット等の設計メモ |
| [implementation-tasks.md](implementation-tasks.md) | 未実装・作りかけを優先度つきタスク化した一覧 |

## 追記の目安

- 世界観・敵の役割・アクションの意図 → ここ（`docs/`）
- 距離・ダメージなどの具体数値 → まずコード、必要なら docs の該当節にも要約
- AI 実装時のコーディング規約 → `.cursor/rules/`（docs には書かない）

テーマが増えたら `ai-behavior.md` などファイルを分け、この目次に追記してください。
