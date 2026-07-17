#pragma once
#include "Components.h"
#include "Types.h"
#include "Chunk.h"
#include "json.hpp"
#include "ComponentField.h"

constexpr int kNotEntity = -1;
constexpr const char* kJsonDataPath = "data";
constexpr const char* kJsonTypeIdPath = "typeId";
constexpr const char* kJsonVirsionPath = "virsion";
constexpr const char* kJsonTypeNamePath = "typeName";



Entity FindEntity(std::string a_key, const nlohmann::json& a_json, const EntityMap& a_entityMap);
int ToEntityIndex(Entity a_targetEntity, const std::vector<Entity>& a_entities);

template<typename Component>
nlohmann::ordered_json NewToJson(Component& a_component, const std::vector<Entity>& a_entities)
{
	// Componentを登録するためのJson
	nlohmann::ordered_json json;
	json[kJsonTypeIdPath] = Component::kTypeId;
	json[kJsonVirsionPath] = Component::kVersion;
	json[kJsonTypeNamePath] = Component::kTypeName;

	// 値をJsonに登録するためのラムダ
	auto findValue = [&](std::string a_name, auto& a_value, const auto& a_defaultValue)
		{
			json[kJsonDataPath][a_name] = a_value;
		};
	// EntityをJsonに登録するためのラムダ
	auto findEntity = [&](std::string a_name, auto& a_value)
		{
			json[kJsonDataPath][a_name] = ToEntityIndex(a_value, a_entities);
		};

	ComponentSystem::ApplyToFields(a_component, findValue, findEntity);
	return json;
}

template<typename Component>
Component NewFromJson(nlohmann::ordered_json a_json, const EntityMap& a_entityMap)
{
	// Componentを作成
	Component component; 
	// 値をComponentに登録するためのラムダ
	auto setValue = [&](std::string a_name, auto& a_value, const auto& a_defaultValue)
		{
			a_value = a_json[kJsonDataPath].value(a_name, a_defaultValue);
		};
	// Entityを探してComponentに登録するためのラムダ
	auto setEntity = [&](std::string a_name, auto& a_value)
		{
			a_value = FindEntity(a_name, a_json[kJsonDataPath], a_entityMap);
		};

	ComponentSystem::ApplyToFields(component, setValue, setEntity);
	return component;
}

namespace ComponentSystem
{
	template<typename Component>
	Component FromJson(const nlohmann::json& a_json, const EntityMap& a_entityMap)
	{
		return NewFromJson<Component>(a_json, a_entityMap);
	}

	template<typename Component>
	nlohmann::ordered_json ToJson(const Component& component, const std::vector<Entity>& entities)
	{
		Component copy = component;
		return NewToJson(copy, entities);
	}
}

class ComponentsSerialize
{
public:
	ComponentsSerialize();
	~ComponentsSerialize() = default;
	// any_castで元に戻す
	using ComponentData = std::any;
	using DeserializeFunc = std::function<ComponentData(const nlohmann::json&, const EntityMap&)>;
	using SerializeFunc = std::function<nlohmann::ordered_json(const void*, const std::vector<Entity>&)>;

	// TODO ここでセット側の情報を知らずに読み込んだファイルをセットしたい
	template<class T>
	void Register(TypeID a_id) {
		factory[a_id] = [](const nlohmann::json& a_json, const EntityMap& entityMap) -> ComponentData {
			return NewFromJson<T>(a_json, entityMap);
			};

		createJson[a_id] = [](const void* component, const std::vector<Entity>& entities) -> nlohmann::ordered_json {
			T copy = *static_cast<const T*>(component);
			return NewToJson(copy, entities);
			};
			
	}

	// factoryから値を取得する
	ComponentData GetData(TypeID a_id, const nlohmann::json& a_json, const EntityMap& a_entityMap) const
	{
		// Tag等のデータが存在しないコンポーネントがあるためデータが存在するか確認
		if (a_json.contains(kJsonDataPath) && !a_json[kJsonDataPath].is_null())
		{
			return factory.at(a_id)(a_json, a_entityMap);
		}
		// データが存在しない場合空のJsonを渡す
		static const nlohmann::json kEmptyData = nlohmann::json::object();
		return factory.at(a_id)(kEmptyData, a_entityMap);
	}

	nlohmann::ordered_json CreateJson(TypeID a_id, const void* component, const std::vector<Entity>& entities) const;

private:
	std::unordered_map<TypeID, SerializeFunc> createJson;
	std::unordered_map<TypeID, DeserializeFunc> factory;
};

void TestRegisterComponent(ComponentsSerialize& a_serialize);

void LoadJsonComponent(Chunk& a_chunk, const ComponentsSerialize& a_serialize, std::string a_key);

nlohmann::ordered_json OutJsonComponents(Chunk& a_chunk, const ComponentsSerialize& a_serialize);