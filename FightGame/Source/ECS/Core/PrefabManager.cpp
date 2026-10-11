#include "PrefabManager.h"

void PrefabManager::CreatePrefabScene(std::string a_sceneName)
{
    prefabScenes.insert({a_sceneName, std::vector<EntityPrefab>()});
}

void PrefabManager::AddPrefab(std::string a_sceneName, EntityPrefab a_prefab)
{
    if(prefabScenes.find(a_sceneName) == prefabScenes.end()) return;

    prefabScenes.at(a_sceneName).push_back(a_prefab);
}

// PrefabScene内容を生成するためのリクエストを取得する
bool PrefabManager::TryGetPrefabScene(std::string a_sceneName, EntitySpawnRequest& a_outRequest)
{
    const auto sceneIt = prefabScenes.find(a_sceneName);
    if(sceneIt == prefabScenes.end()) return false;

    a_outRequest.prefabs = sceneIt->second;
    return true;
}
