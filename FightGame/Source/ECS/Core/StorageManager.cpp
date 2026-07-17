#include "StorageManager.h"



StorageManager::StorageManager()
{
}

StorageManager::~StorageManager()
{
}

HasView StorageManager::GetHasView(Entity a_entity)
{
	return HasView(a_entity, storages);
}

void StorageManager::DeleteComponent(Entity a_entity, const TypeID a_type)
{
	// ‘Î‰ž‚·‚éstorage‚ª‘¶Ý‚µ‚È‚¯‚ê‚Î•Ô‚·
	auto it = storages.find(a_type);
	if (it == storages.end()) return;

	auto storage = storages.at(a_type);
	storage->Delete(a_entity);

}


