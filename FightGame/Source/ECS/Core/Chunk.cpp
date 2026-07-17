#include "Chunk.h"
#include "HasView.h"
#include "imGui/imgui.h"
#include "Debug.h"

#define CreateAndAddComponent(chunk, ...) 


Chunk::Chunk()
	:entitesManager()
	,storageManager()
{
}

void Chunk::ImGuiInPut()
{

	if (ImGui::Button("Create Entity", ImVec2(100.0f, 100.0f)))
	{
		this->CreateNewEntity();
	}

	if (auto result = entitesManager.ImGuiUseEntity())
	{
		ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 100), ImGuiWindowFlags_NoTitleBar);

		HasView view = storageManager.GetHasView(result.value());

		for (auto it : view)
		{
			ImGui::Text(std::to_string(it).c_str());
		}

		ImGui::EndChild();
	}
	
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


