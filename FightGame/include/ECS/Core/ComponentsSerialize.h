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

// *******************************
// 処理の流れメモ by 越智晴彦 
// 1.生成、登録
// コンストラクタ、デストラクタでは何もしません。
// 生成後、登録、読み込みを行うRegisterByTypeを呼び出し、各コンポーネントの生成関数を登録する必要があります。
// 内部ではコンポーネント展開マクロであるCOMPONENT_TYPE_LIST(現在Components.hに記述)を使用しているためそちら側に登録していなければ読み込めません
// 
// 2.Jsonへの保存、Jsonから取得
// 上記で登録した生成関数を活用し、双方向の変換を行います。
// 
// Jsonへの保存はコンポーネント本体とコンポーネントのTypeID、参照を持たせたいEntity群を渡すことでTypeId、Verson、TypeNameを保存し、その下の層に
// ApplyToFieldsにより展開を行い、各データを登録できます。
// 
// コンポーネントtの取得はJsonデータと受け取りたいコンポーネントのTypeID、参照を持たせたいEntity群を渡すことで登録されたに内容のコンポーネントを
// ApplyToFieldsにより読み込みを行い、std::anyで受け取ることができます。
// 
// また、LoadJsonComponent及びOutJsonComponentにより、Chunk単位での登録、読み込みが行えるため、基本こっちを使うと思います。
// n.Entity群の扱い方について
//




template<typename Component>
nlohmann::ordered_json NewToJson(Component& a_component, const std::vector<Entity>& a_entities)
{
	// Componentを登録するためのJson
	nlohmann::ordered_json json;
	// コンポーネントそのものの基本情報を登録する
	json[kJsonTypeIdPath] = Component::kTypeId;
	json[kJsonVirsionPath] = Component::kVersion;
	json[kJsonTypeNamePath] = Component::kTypeName;

	// 値をJsonに登録するためのラムダ
	auto findValue = [&](std::string a_name, auto& a_value, const auto& a_defaultValue)
		{
			// コンポーネントの1階層下にデータを書き込む
			json[kJsonDataPath][a_name] = a_value;
		};
	// EntityをJsonに登録するためのラムダ
	auto findEntity = [&](std::string a_name, auto& a_value)
		{
			// 参照を持てるEntity群を受け取り再度読み取れる形で保存
			json[kJsonDataPath][a_name] = ToEntityIndex(a_value, a_entities);
		};

	// templateの登録関数で型ごとに登録する
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

	// templateの登録関数で型ごとに読み込む
	ComponentSystem::ApplyToFields(component, setValue, setEntity);
	return component;
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
		// 渡された型のidをキーとし、factoryにJsonとコンポーネント双方の切り替え機能を登録する

		// Componentへ変換するラムダを登録
		factory[a_id] = [](const nlohmann::json& a_json, const EntityMap& entityMap) -> ComponentData {
			return NewFromJson<T>(a_json, entityMap);
			};
 
		// Jsonへ変換するラムダを登録、保存の都合でvoid*で受け取る
		createJson[a_id] = [](const void* component, const std::vector<Entity>& entities) -> nlohmann::ordered_json {
			T copy = *static_cast<const T*>(component);
			return NewToJson(copy, entities);
			};
			
	}

	// Jsonからコンポーネントを生成する。
	ComponentData GetData(TypeID a_id, const nlohmann::json& a_json, const EntityMap& a_entityMap) const;
	
	// Component情報をJsonに保存して返す。
	nlohmann::ordered_json CreateJson(TypeID a_id, const void* component, const std::vector<Entity>& entities) const;

private:
	std::unordered_map<TypeID, SerializeFunc> createJson;
	std::unordered_map<TypeID, DeserializeFunc> factory;
};

void TestRegisterComponentType(ComponentsSerialize& a_serialize);

std::vector<Entity> LoadJsonComponent(Chunk& a_chunk, const ComponentsSerialize& a_serialize, std::string a_key);

nlohmann::ordered_json OutJsonComponents(Chunk& a_chunk, const ComponentsSerialize& a_serialize);