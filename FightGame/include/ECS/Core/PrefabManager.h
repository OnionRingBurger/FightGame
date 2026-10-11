#ifndef ___PREFAB_MANAGER_H___
#define ___PREFAB_MANAGER_H___

#include <unordered_map>
#include <any>
#include <string>
#include <vector>

#include "ECSTypes.h"

// Prefab—p‚ÌScene‚ğì¬‚µAŠÇ—‚ğs‚¤
class PrefabManager
{
public:
	PrefabManager() = default;
	~PrefabManager() = default;

	// Prefab‚ğŠÇ—‚·‚é‚½‚ß‚Ì“à•”Scene‚ğì¬‚·‚é
	void CreatePrefabScene(std::string a_sceneName);
	// ì¬‚µ‚½Scene‚ÉPrefab‚ğ“o˜^‚·‚é
	void AddPrefab(std::string a_sceneName, EntityPrefab a_prefab);

	bool TryGetPrefabScene(std::string a_sceneName, EntitySpawnRequest& a_outRequest);
private:
	std::unordered_map<std::string, std::vector<EntityPrefab>> prefabScenes;
};

#endif
