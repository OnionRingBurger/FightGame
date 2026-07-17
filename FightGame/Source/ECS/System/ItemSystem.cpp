#include "ItemSystem.h"
#include "Components.h"

using namespace Component;

void ItemGetSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	// ぶつかったら取得されるItemを受け取る
	ComponentView view = a_chunk.GetView<ComponentTypes<ItemTag, OwnerComponent, HitInfomation>>();
	// すべて処理
	for (auto it : view)
	{
		// 保持者を登録する用のコンポーネントを受け取る
		ComponentHandle<OwnerComponent> owner = a_chunk.GetComponent<OwnerComponent>(it);
		// 当たったEntityを保持者とするため情報を取得
		const ComponentHandle<HitInfomation> info = a_chunk.GetComponent<HitInfomation>(it);

		for (auto infoIt : info.Look().triggerResults)
		{
			// 判定用にComponentに変更を加えずEntity情報を受け取る
			Entity triggerEntity = infoIt.triggerEntity;

			// Entityが不正な値だった場合処理しない
			if (triggerEntity.id == -1 || triggerEntity.generation == -1) continue;

			// TODO Componentが必要なTagを持ってるか判定したほうがいい

			// マップ上に存在するためのコンポーネントを削除し保持されたアイテムとして扱う
			// TODO 後からマップに存在するためのアイテムが増えるかもしれないので書き方を変えた方がいいかも、後必要ならちゃんとコンポーネントが存在するか確認する
			a_chunk.DeleteChunkComponent(it, Position::kTypeId);
			a_chunk.DeleteChunkComponent(it, Rotation::kTypeId);
			a_chunk.DeleteChunkComponent(it, Scale::kTypeId);
			a_chunk.DeleteChunkComponent(it, BoxCollider::kTypeId);
			a_chunk.DeleteChunkComponent(it, OBBCollider::kTypeId);
			a_chunk.DeleteChunkComponent(it, HitInfomation::kTypeId);

			// 保持者を登録
			owner->owner = triggerEntity;
		}
	}
}