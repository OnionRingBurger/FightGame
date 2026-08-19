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

	ImGui::Combo("UseEntity", &useEntityCount, names.data(), names.size());
	
	Entity useEntity = entities.at(useEntityCount);

	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 100), ImGuiWindowFlags_NoTitleBar);


	HasView view = storageManager.GetHasView(useEntity);
	for (auto it : view)
	{
		// !!!New!!!
		// ApplyToFields ‚Ö‚ÍƒI[ƒo[ƒ[ƒh‰ğŒˆ‚Ì‚½‚ßƒ‰ƒ€ƒ_‚Å•ï‚ñ‚Å“n‚·iSerialize ‚Ì NewToJson ‚Æ“¯‚¶Œ`j
		#define IMGUI_COMPONENT_CASE(T) case T::kTypeId: { \
		ComponentHandle<T> handle = this->GetComponent<T>(useEntity); \
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

	TypeID addId;

	addId = Position::kTypeId;

	#define IMGUI_ADDCOMPONENT_CASE(T) case T::kTypeId: {\
		this->AddComponent(useEntity, T()); \
	}\
	break; \
	
	if (ImGui::Button("AddComponent", ImVec2(100.0f, 20.0f)))
	{
		switch (addId)
		{
			COMPONENT_TYPE_LIST(IMGUI_ADDCOMPONENT_CASE)
		}
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


