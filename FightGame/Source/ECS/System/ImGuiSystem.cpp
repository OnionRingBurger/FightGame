#include "ImGuiSystem.h"
#include "Components.h"
#include "DebugSystemResponse.h"
#include "imGui/imgui.h"
#include "Defines.h"
#include "json.hpp"
#include <fstream>
#include <vector>
#include <string>

using namespace Component;

void ImGuiSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ImGui::SetNextWindowPos(ImVec2(50.0f, 50.0f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300.0f, 350.0f), ImGuiCond_Once);
	ImGui::Begin("ChunkWindow");

	a_chunk.ImGuiInPut();

	a_chunk.ImGuiOutPut();

	ImGui::End();

}

void SaveWorldSystem(Chunk& a_chunk, const SystemContext& a_context, SystemResponse& response)
{
	ImGui::SetNextWindowPos(ImVec2(50.0f, 600.0f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(220.0f, 220.0f), ImGuiCond_Once);
	ImGui::Begin("Test");

	// !!!New!!!
	static int selectedKey = 0;
	std::vector<std::string> keyStorage;
	std::vector<const char*> keyPtrs;
	{
		std::ifstream ifs(kDataPath);
		if (ifs.is_open())
		{
			nlohmann::json json;
			ifs >> json;
			for (auto it = json.begin(); it != json.end(); ++it)
			{
				if (it.value().is_array())
				{
					keyStorage.push_back(it.key());
				}
			}
		}
	}
	for (auto& key : keyStorage)
	{
		keyPtrs.push_back(key.c_str());
	}
	if (selectedKey < 0 || selectedKey >= (int)keyPtrs.size())
	{
		selectedKey = 0;
	}
	if (!keyPtrs.empty())
	{
		ImGui::Combo("DataKey", &selectedKey, keyPtrs.data(), (int)keyPtrs.size());
	}

	const std::string selectedName = keyPtrs.empty() ? std::string("Stage1Phase1") : keyStorage[(size_t)selectedKey];
	DebugSystemResponse& debugSystemResponse = dynamic_cast<DebugSystemResponse&>(response);

	if (ImGui::Button("SaveEntity", ImVec2(150.0f, 30.0f)))
	{
		debugSystemResponse.SaveRequest(selectedName);

		ComponentView view = a_chunk.GetView<ComponentTypes<DebugEntityTag>>();
		for (auto it : view)
		{
			a_chunk.DeleteChunkEntity(it);
		}
	}

	// !!!New!!!
	if (ImGui::Button("LoadEntity", ImVec2(150.0f, 30.0f)))
	{
		debugSystemResponse.LoadRequest(selectedName);

		std::vector<Entity> entities = a_chunk.GetAllEntity();
		for (auto it : entities)
		{
			a_chunk.DeleteChunkEntity(it);
		}
	}

	ImGui::End();
}

void UndoWorldSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	Entity debugCamera = a_chunk.CreateNewEntity(
		DebugCameraTag(),
		DebugEntityTag(),
		Camera(10, float3(0.0f, 0.0f, 1.0f), float3(0.0f, 1.0f, 0.0f), 60.0f, 1.77777f, 0.05f, 1000.0f),
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		PosePosState(POSE_POS_DEBUGCAMERA),
		PoseRotState(POSE_ROT_DEBUGCAMERA)
	);

	Entity debugCameraPoint = a_chunk.CreateNewEntity(
		MOVE_AND_TRANSFORM_COMPONENT(
			float3(0.0f, 0.0f, 0.0f),
			float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 1.0f, 1.0f)
		),
		DebugEntityTag(),
		InputMove(),
		InputRotato(),
		CameraPoint(1000000000, 10000.0f, 10000.f)
	);
}
