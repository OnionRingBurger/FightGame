#include "EntitySpawnPort.h"
#include "Chunk.h"
#include "ComponentField.h"

#include <map>

using namespace ComponentSystem;

std::vector<Entity> EntitySpawnPort::AddEntities(Chunk& a_chunk, const EntitySpawnRequest& a_request)
{
    std::vector<Entity> ret;
    // PrefabとRuntimeのEntityの対応関係を作成
    std::unordered_map<Entity, Entity> entitiesMap;
    for(auto prefab : a_request.prefabs)
    {
        Entity runtimeEntity = a_chunk.CreateNewEntity(); 
        entitiesMap.insert({prefab.entity, runtimeEntity});
        ret.push_back(runtimeEntity);
    }

    // 各コンポーネントを追加
    for(auto prefab : a_request.prefabs)
    {
        Entity runtimeEntity = entitiesMap.at(prefab.entity);
        for(auto componentData : prefab.haveComponents)
        {
            switch (componentData.id) 
            {

                // Entityへの参照を保持している場合はRuntimeへ置き換えしてコンポーネントを登録する
                #define ADD_ANY_COMPONENT(T) case T::kTypeId: {\
                T component = std::any_cast<T>(componentData.componrnt); \
                auto dummyFunc = [](std::string, auto&, const auto&){};\
                auto toRuntimeFunc = [&](std::string name, Entity& entityRef){entityRef = runtimeEntity;};\
                ApplyToFields(component, dummyFunc, toRuntimeFunc);\
                a_chunk.AddComponent(runtimeEntity, component);\
                } break;

                // コンポーネントに応じて追加する
                COMPONENT_TYPE_LIST(ADD_ANY_COMPONENT)

                #undef ADD_ANY_COMPONENT
            }
        }   
    }

    return ret;
}
