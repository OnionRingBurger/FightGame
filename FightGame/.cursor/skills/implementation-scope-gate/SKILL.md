---
name: implementation-scope-gate
description: 実装依頼時にAskQuestionで編集スコープを確認してからプロジェクトソースを触る手順。World/Components.h/既存SystemはデフォルトOFF。Rules/Skills更新直後は rules-skills-consistency-check も実行。
---

# 実装スコープ確認ゲート

エージェント側の既定手順。ユーザーに依頼文の書き方を求めない。

Rule: [`.cursor/rules/implementation-scope-gate.mdc`](../../rules/implementation-scope-gate.mdc)、[`.cursor/rules/edit-approval.mdc`](../../rules/edit-approval.mdc)

## いつ読むか

- 「実装して」「実装しろ」等の **プロジェクトソース** 実装依頼
- 要承認ファイル（World、`Components.h`、依頼対象外の既存 System）に触れうる作業の直前

## 手順

### 1. 編集対象の洗い出し

依頼内容から触る可能性のあるファイルを列挙し、3群に分ける。

| 群 | 例 |
|----|-----|
| 承認不要 | 依頼で新規指定された `.h`/`.cpp` ペア、`register-source.ps1`、`System.h` への include（Rules 義務） |
| 要承認・未承認 | `include/ECS/World` / `Source/ECS/World`、`Components.h`、依頼外の既存 System、`GameScene` 等 |
| 今回触らない | 依頼に無い World、配線先、JSON（ユーザー担当と明言されたもの） |

### 2. AskQuestion（必須）

**Write / StrReplace の前に** 必ず出す。要承認グループは **選択肢に含めてもデフォルトは未選択**（ユーザーが明示選択するまで触らない）。

質問例:

- **title**: 今回の実装スコープ
- **prompt**: このターンで編集してよいファイルを選んでください（複数可）。選ばれなかった要承認ファイルは編集しません。
- **options**（依頼に応じて具体化）:
  - 新規ペアのみ（例: `GameSystemResponse.h/.cpp`）
  - World 配下（`ProtoWorld.cpp` 等）— 要承認
  - `Components.h` / `ComponentField.h` — 要承認
  - 既存 System（例: `EffectSystem.cpp`）— 要承認
  - `GameScene.cpp` — 要承認
  - 今回は触らない（説明・調査のみ）

`allow_multiple: true` を使う。

### 3. 回答後のみ編集

- 選ばれたファイル **だけ** Write / StrReplace
- 未選択の要承認は **「続きには ○○ の選択が必要」** と報告して止める
- Entity / Component 列の変更は、ファイル承認に加え **Component 名の明示** が無ければ行わない（`edit-approval`）

### 4. 完了報告

- 編集したファイル一覧
- 未選択で未実装の部分
- コンパイル未確認の可能性

## AskQuestion が不要な場合

- Rules / Skills / 計画書 **のみ** の変更
- **同ターン** のユーザー発話に編集可パスが列挙されている（例: `ProtoWorld.cpp を編集してよい`）
- **直前ターン** の AskQuestion 回答が有効で、スコープ外の追加が無い

## Rules / Skills を更新した直後

個人スキル `rules-skills-consistency-check`（`~/.cursor/skills/rules-skills-consistency-check/SKILL.md`）で整合性監査を行い、矛盾があれば報告する。

## 監査チェックリスト（実装スコープ関連）

- [ ] `implementation-scope-gate.mdc` と `edit-approval.mdc` で「実装して ≠ 編集許可」が一致しているか
- [ ] AskQuestion 必須と「同ターン要承認編集禁止」が矛盾していないか
- [ ] World / Components.h のデフォルト OFF が両方に書いてあるか
- [ ] スキル内の Rule 参照パスが実在するか
