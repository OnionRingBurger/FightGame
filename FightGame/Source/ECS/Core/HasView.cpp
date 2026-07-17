#include "HasView.h"



HasView::HasView(Entity a_entity, std::unordered_map<TypeID, std::shared_ptr<IComponentStorage>>& a_storages)
	: storage()
{
	
	// 受け取ったストレージのmapを探索
	for (auto it : a_storages)
	{
		// 指定されたEntityが持っていなければComponentを持っていなければ次へ
		if (!it.second->Has(a_entity)) continue;

		// 持っていたので読み込む
		storage.push_back(it.first);
		
	}

}

HasView::HasView()
	: storage()
{
}


HasView::iterator HasView::begin()
{
	return storage.begin();
}

HasView::iterator HasView::end()
{
	return storage.end();
}


