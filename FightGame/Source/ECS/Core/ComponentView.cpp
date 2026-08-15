#include "ComponentView.h"




ComponentView::ComponentView(std::vector<std::vector<Entity>>& a_entities)
{

	std::vector<Entity> trueEntities;

	std::sort(a_entities.begin(), a_entities.end(), [](const std::vector<Entity>& a, const std::vector<Entity> b) {return a.size() < b.size(); });

	for (auto& entitiesIt : a_entities)
	{
		if (&entitiesIt == &a_entities.at(0))
		{
			trueEntities = a_entities.at(0);
			continue;
		}

		iterator entityIt, entityEnd;
		iterator trueIt, trueEnd;
		std::vector<Entity> cmpEntities;

		for (entityIt = entitiesIt.begin(), entityEnd = entitiesIt.end(); entityIt != entityEnd; ++entityIt)
		{
			for (trueIt = trueEntities.begin(), trueEnd = trueEntities.end(); trueIt != trueEnd; ++trueIt)
			{
				if (!(*trueIt == *entityIt)) continue;
				
				cmpEntities.push_back(*entityIt);
				break;
			}
			
		}
		
		trueEntities = cmpEntities;

	}
	
	entities = trueEntities;
}

ComponentView::ComponentView(std::vector<std::vector<Entity>>& a_entities, std::vector<std::vector<Entity>>& a_exclusionEntities)
{
	// 条件に合致したEntity
	std::vector<Entity> trueEntities;

	std::sort(a_entities.begin(), a_entities.end(), [](const std::vector<Entity>& a, const std::vector<Entity> b) {return a.size() < b.size(); });

	// 渡されたすべてのEntityを確認し、重複したEntityだけ残す
	for (auto& entitiesIt : a_entities)
	{
		if (&entitiesIt == &a_entities.at(0))
		{
			trueEntities = a_entities.at(0);
			continue;
		}

		iterator entityIt, entityEnd;
		iterator trueIt, trueEnd;
		std::vector<Entity> cmpEntities;

		for (entityIt = entitiesIt.begin(), entityEnd = entitiesIt.end(); entityIt != entityEnd; ++entityIt)
		{
			for (trueIt = trueEntities.begin(), trueEnd = trueEntities.end(); trueIt != trueEnd; ++trueIt)
			{
				if (!(*trueIt == *entityIt)) continue;

				cmpEntities.push_back(*entityIt);
				break;
			}

		}

		trueEntities = cmpEntities;

	}

	// std::sort(a_exclusionEntities.begin(), a_exclusionEntities.end(), [](const std::vector<Entity>& a, const std::vector<Entity> b) {return a.size() > b.size(); });

	// 削除対象Entityに登録されているか確認
	for (auto& entitiesIt : a_exclusionEntities)
	{
		iterator entityIt, entityEnd;
		iterator trueIt, trueEnd;
		std::vector<Entity> cmpEntities;

		// tureEntityが削除対象になっていないか全て確認
		for (trueIt = trueEntities.begin(), trueEnd = trueEntities.end(); trueIt != trueEnd; ++trueIt)
		{
			bool isExclusion = false;
			for (entityIt = entitiesIt.begin(), entityEnd = entitiesIt.end(); entityIt != entityEnd; ++entityIt)
			{
				if (*entityIt == *trueIt)
				{
					isExclusion = true;
					break;
				}
			}
			// 削除対象じゃなかったらcmpに追加しない
			if (isExclusion) continue;
			// 追加
			cmpEntities.push_back(*trueIt);
		}

		// trueEntityを更新
		trueEntities = cmpEntities;

	}

	entities = trueEntities;
}



ComponentView::iterator ComponentView::begin()
{
	return entities.begin();
}

ComponentView::iterator ComponentView::end()
{
	return entities.end();
}