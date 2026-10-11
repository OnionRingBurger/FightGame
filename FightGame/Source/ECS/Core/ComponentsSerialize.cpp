#include "ComponentsSerialize.h"
#include <fstream>
#include <any>
#include <stdexcept>
#include "Defines.h"

template<typename Component>
void JsonAddComponent(Entity a_entity, Chunk& a_chunk, const nlohmann::json& a_json, const ComponentsSerialize& a_serialize, const EntityMap& a_entityMap);
const void* GetIdComponent(TypeID a_id, Chunk& a_chunk, Entity a_entity);

using SerializeComponentData = std::any;


// 渡されたコンポーネントIDに対応する型をSerializeに登録する
void RegisterByTypeId(ComponentsSerialize& a_serialize, TypeID a_id)
{
	switch (a_id)
	{
		// 全コンポーネント分の登録caseを展開する
#define REGISTER_CASE(T) case T::kTypeId: a_serialize.Register<T>(T::kTypeId); break;
		COMPONENT_TYPE_LIST(REGISTER_CASE)
#undef REGISTER_CASE
	}
}

// 渡されたIDに対応する
void AddComponentByTypeId(
	TypeID a_id,
	Entity a_entity,
	Chunk& a_chunk,
	const nlohmann::json& a_json,
	const ComponentsSerialize& a_serialize,
	const EntityMap& a_entityMap)
{
	switch (a_id)
	{
		// 全コンポーネント分のcaseを展開する
#define LOAD_CASE(T) case T::kTypeId: JsonAddComponent<T>(a_entity, a_chunk, a_json, a_serialize, a_entityMap); break;
		COMPONENT_TYPE_LIST(LOAD_CASE)
#undef LOAD_CASE
	}
}

Entity FindEntity(std::string a_key, const nlohmann::json& a_json, const EntityMap& a_entityMap)
{
	int targetId = a_json.value(a_key, kNotEntity);

	Entity target = kInvalidEntity;
	if (targetId != kNotEntity && a_entityMap.find(targetId) != a_entityMap.end())
	{
		target = a_entityMap.at(targetId);
	}

	return target;
}

int ToEntityIndex(Entity a_targetEntity, const std::vector<Entity>& a_entities)
{
	int index = 0;
	for (auto it : a_entities)
	{
		if (a_targetEntity == it)
		{
			return index;
		}
		index++;
	}
	return kNotEntity;
}

void TestRegisterComponentType(ComponentsSerialize& a_serialize)
{
#define REGISTER_ALL(T) RegisterByTypeId(a_serialize, T::kTypeId);
	COMPONENT_TYPE_LIST(REGISTER_ALL)
#undef REGISTER_ALL
}

ComponentsSerialize::ComponentsSerialize()
{

}

SerializeComponentData ComponentsSerialize::GetData(TypeID a_id, const nlohmann::json& a_json, const EntityMap& a_entityMap) const
{
	// Tag等のデータが存在しないコンポーネントがあるためデータが存在するか確認
	if (a_json.contains(kJsonDataPath) && !a_json[kJsonDataPath].is_null())
	{
		// 存在する場合データを使って生成
		return factory.at(a_id)(a_json, a_entityMap);
	}
	// データが存在しない場合空のJsonを渡す
	static const nlohmann::json kEmptyData = nlohmann::json::object();
	return factory.at(a_id)(kEmptyData, a_entityMap);
}

nlohmann::ordered_json ComponentsSerialize::CreateJson(TypeID a_id, const void* a_component, const std::vector<Entity>& a_entities) const
{
	return createJson.at(a_id)(a_component, a_entities);
}

std::vector<Entity> LoadJsonComponent(Chunk& a_chunk, const ComponentsSerialize& a_serialize, std::string a_key)
{
	std::vector<Entity> ret;

	// ファイルを読み込む
	std::ifstream stream(kDataPath);
	nlohmann::json json;
	stream >> json;

	// 予めすべての	Entityを作成
	EntityMap entityMap;
	for (auto itEntity : json[a_key])
	{
		Entity newEntity = a_chunk.CreateNewEntity();
		entityMap[itEntity["number"]] = newEntity;
		ret.push_back(newEntity);
	}

	// 作成したエンティティにコンポーネントを追加
	for (auto itEntity : json[a_key])
	{
		Entity entity = entityMap[itEntity["number"]];
		for (auto itComponents : itEntity["components"])
		{
			const TypeID typeId = static_cast<TypeID>(itComponents.value(kJsonTypeIdPath, static_cast<int>(kNotEntity)));
			AddComponentByTypeId(typeId, entity, a_chunk, itComponents, a_serialize, entityMap);
		}
	}

	return ret;
}

const void* GetIdComponent(TypeID a_id, Chunk& a_chunk, Entity a_entity)
{
	const void* ret = nullptr;
	switch (a_id)
	{
#define GET_COMPONENT_CASE(T) case T::kTypeId: ret = &a_chunk.GetComponent<T>(a_entity).Look(); break;
		COMPONENT_TYPE_LIST(GET_COMPONENT_CASE)
#undef GET_COMPONENT_CASE
	default:
		break;
	}

	return ret;
}

nlohmann::ordered_json OutJsonComponents(Chunk& a_chunk, const ComponentsSerialize& a_serialize)
{
	// データ格納用にクラスを作成し全てのエンティティを取得
	nlohmann::ordered_json sceneData;
	std::vector<Entity> entities = a_chunk.GetAllEntity();

	// 走査
	for (int i = 0; i < entities.size(); i++)
	{
		// 各entityごとに走査しデータを取得、保存
		nlohmann::ordered_json entityData;
		entityData["number"] = i;
		HasView view = a_chunk.GetHasView(entities.at(i));
		for (auto it : view)
		{
			entityData["components"].push_back(a_serialize.CreateJson(it, GetIdComponent(it, a_chunk, entities[i]), entities));
		}
		sceneData.push_back(entityData);
	}

	return sceneData;
}

template<typename Component>
void JsonAddComponent(Entity a_entity, Chunk& a_chunk, const nlohmann::json& a_json, const ComponentsSerialize& a_serialize, const EntityMap& a_entityMap)
{
	a_chunk.AddComponent(a_entity, std::any_cast<Component>(a_serialize.GetData(a_json["typeId"], a_json, a_entityMap)));
}



std::unordered_map<std::string, std::vector<EntityPrefab>> LoadAllPrefab(const nlohmann::json& a_json, const ComponentsSerialize& a_serialize)
{
	std::unordered_map<std::string, std::vector<EntityPrefab>> ret;

	std::unordered_map<int, int> numberMap;

	for (auto& [sceneName, entities] : a_json.items())
	{
		ret.insert({ sceneName, LoadPrefab(a_json, a_serialize, sceneName)});
	}
	
	return ret;
}




std::vector<EntityPrefab> LoadPrefab(const nlohmann::json& a_json, const ComponentsSerialize& a_serialize, std::string a_key)
{
	std::vector<EntityPrefab> ret;

	std::unordered_map<int, int> numberMap;

	// 予めすべての	Entityを作成
	EntityMap entityMap;
	for (auto itEntity : a_json[a_key])
	{
		int number = itEntity["number"];
		Entity newEntity = Entity{ (size_t)number, 0 };
		entityMap[number] = newEntity;
		numberMap.insert({number, ret.size()});
		ret.push_back(EntityPrefab(newEntity, std::vector<ComponentData>()));

	}


	// 作成したエンティティにコンポーネントを追加
	for (auto itEntity : a_json[a_key])
	{
		int number = itEntity["number"];
		for (auto itComponents : itEntity["components"])
		{
			const TypeID typeId = static_cast<TypeID>(itComponents.value(kJsonTypeIdPath, static_cast<int>(kNotEntity)));
			SerializeComponentData data = a_serialize.GetData(itComponents["typeId"], itComponents, entityMap);

			ret.at(numberMap.at(number)).haveComponents.push_back(ComponentData(typeId,data));
		}
	}

	return ret;
}

#undef COMPONENT_TYPE_LIST


