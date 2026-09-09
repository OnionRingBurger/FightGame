#include "Chunk.h"
#include "HasView.h"
#include "imGui/imgui.h"
#include "Debug.h"
#include "Components.h"
#include "ComponentField.h"

#define CreateAndAddComponent(chunk, ...) 
using namespace Component;
using namespace ComponentSystem;

Chunk::Chunk()
	:entitesManager()
	,storageManager()
{
}

void Chunk::ImGuiInPut()
{

	if (ImGui::Button("Create New Entity", ImVec2(150.0f, 30.0f)))
	{
		Entity entity = this->CreateNewEntity();
		this->AddComponent(entity, Name("NewEntity"));
	}

	if (ImGui::Button("Create Move Entity", ImVec2(150.0f, 30.0f)))
	{
		Entity entity = this->CreateNewEntity(
			MOVE_AND_TRANSFORM_COMPONENT(
				float3(0.0f, 0.0f, 0.0f),
				float3(0.0f, 0.0f, 0.0f),
				float3(1.0f, 1.0f, 1.0f)
			)
		);
		this->AddComponent(entity, Name("NewEntity"));
	}


	static int useEntityCount = 0;

	std::vector<Entity> entities = entitesManager.GetAllEntity();
	std::vector<string> namesString;
	std::vector<const char*> names;
	if (entities.empty())
	{
		return;
	}
	
	int i = 0;
	for (auto it : entitesManager.GetAllEntity())
	{

		ComponentHandle<Name> nameComp = this->GetComponent<Name>(it);
		namesString.push_back(nameComp.IsValid() ? nameComp.Look().name : "Entity : " + to_string(i));
		
		i++;
	}

	for (auto& it : namesString)
	{
		names.push_back(it.c_str());
	}

	static char nameBuffer[64] = "";
	ImGui::InputText("ComponentSearch", nameBuffer, 64);
	std::string searchName = nameBuffer;


	ImGui::Combo("UseEntity", &useEntityCount, names.data(), (int)names.size());
	
	Entity useEntity = entities.at(useEntityCount);

	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 100), ImGuiWindowFlags_NoTitleBar);


	HasView view = storageManager.GetHasView(useEntity);
	for (auto it : view)
	{
		// ApplyToFields ‚Ö‚ÍƒI[ƒo[ƒ[ƒh‰ğŒˆ‚Ì‚½‚ßƒ‰ƒ€ƒ_‚Å•ï‚ñ‚Å“n‚·
		#define IMGUI_COMPONENT_CASE(T) case T::kTypeId: { \
		ComponentHandle<T> handle = this->GetComponent<T>(useEntity);\
		std::string selectedName = T::kTypeName; \
		if(searchName.compare(0, searchName.size(), selectedName.substr(0, searchName.size())) != 0) break; \
		ImGui::Text(T::kTypeName);\
		auto imguiValue = [](std::string a_name, auto& a_value, const auto& a_defaultValue) \
		{ \
			CreateImGuiValue(a_name + "(" + T::kTypeName + ")", a_value, a_defaultValue); \
		}; \
		auto imguiEntity = [](std::string a_name, Entity& a_entity) \
		{ \
			CreateImGuiEntity(a_name + "(" + T::kTypeName + ")", a_entity); \
		}; \
		ApplyToFields(*handle.operator->(), imguiValue, imguiEntity); \
		} break;\
	
		switch (it)
		{
			COMPONENT_TYPE_LIST(IMGUI_COMPONENT_CASE)
		}

		#undef IMGUI_COMPONENT_CASE
	}
	ImGui::EndChild();

	static int selectedIdKey = 0;
	std::map<int, TypeID> typeMap;

	std::vector<const char*> typeNames;

#define IMGUI_SELECT_TYPEIDBOX(T) {auto boxFunc = [&](){\
		std::string selectedName = T::kTypeName; \
		if(searchName.compare(0, searchName.size(), selectedName.substr(0, searchName.size())) != 0) return; \
		typeNames.push_back(T::kTypeName);\
		typeMap.insert({typeNames.size() - 1, T::kTypeId});\
	};\
	boxFunc(); \
	}


	COMPONENT_TYPE_LIST(IMGUI_SELECT_TYPEIDBOX);


	for (int i = 0; i < typeNames.size(); i++)
	{

		for (int j = 0; j < typeNames.size(); j++)
		{
			if (i == j) continue;
			if(typeNames.at(i) == typeNames.at(j))
			{
				ImGui::Text("Duplicate TypeName : %s", typeNames.at(i));

			}

			if(typeMap.at(i) == typeMap.at(j))
			{
				ImGui::Text("Duplicate TypeID : %d", typeMap.at(i));
			}
		}

		
		//ImGui::Text(typeNames.at(i));
		//ImGui::Text("%d",typeMap.at(i));

	}

	ImGui::Combo("SelectAddComponent", &selectedIdKey, typeNames.data(), (int)typeNames.size());



	TypeID addId;
	if (typeMap.find(selectedIdKey) == typeMap.end()) return;
	addId = typeMap.at(selectedIdKey);


	#define IMGUI_ADDCOMPONENT_CASE(T) case T::kTypeId: {\
		ComponentHandle<T> handle = this->GetComponent<T>(useEntity);\
		if(!handle.IsValid())\
		{\
			this->AddComponent(useEntity, T()); \
		}\
	}\
	break; \
	
	if (ImGui::Button("AddComponent", ImVec2(100.0f, 20.0f)))
	{
		switch (addId)
		{
			std::cout << "AddComponent : " << addId << std::endl;
			COMPONENT_TYPE_LIST(IMGUI_ADDCOMPONENT_CASE)
		}
	}


#define IMGUI_DELETECOMPONENT_CASE(T) case T::kTypeId: {\
		this->DeleteChunkComponent(useEntity, T::kTypeId); \
	}\
	break; \

	if (ImGui::Button("DeleteComponent", ImVec2(100.0f, 20.0f)))
	{
		switch (addId)
		{
			COMPONENT_TYPE_LIST(IMGUI_DELETECOMPONENT_CASE)
		}
	}

	if (ImGui::Button("DeleteEntity", ImVec2(100.0f, 20.0f)))
	{
		this->DeleteChunkEntity(useEntity);
	}

	#undef IMGUI_ADDCOMPONENT_CASE
}

std::vector<Entity> Chunk::GetAllEntity()
{
	return entitesManager.GetAllEntity();
}

HasView Chunk::GetHasView(Entity a_entity)
{
	if (!entitesManager.IsEntityIsAlive(a_entity))
	{
		return HasView();
	}

	return storageManager.GetHasView(a_entity);
}

void Chunk::ImGuiOutPut()
{
	entitesManager.DefaultImGuiOutPut();
}

void Chunk::DeleteChunkComponent(Entity a_entity, TypeID a_typeId)
{
	// íœ—\’è‚ÌComponentType‚Æ‘Î‰‚·‚éEntity‚ğ“o˜^
	deleteComponents.push_back(std::pair<Entity, TypeID>(a_entity, a_typeId));
}

void Chunk::DeleteChunkEntity(Entity a_entities)
{
	// íœ—\’è‚ÌEntity‚ğ“o˜^
	deleteEntities.push_back(a_entities);
}

void Chunk::CleanupFrame()
{
	// Component‚ğíœ
	for (auto& it : deleteComponents)
	{
		storageManager.DeleteComponent(it.first, it.second);
	}

	// Entities‚ğíœ
	for (auto it : deleteEntities)
	{
		
		HasView view = storageManager.GetHasView(it);
		
		for (auto componentType : view)
		{
			storageManager.DeleteComponent(it, componentType);
		}

		entitesManager.DeleteEntity(it);
	}
	
	deleteComponents.clear();
	deleteEntities.clear();
}


