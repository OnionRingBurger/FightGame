#include "ImGuiSystem.h"
#include "Components.h"
#include "imGui/imgui.h"

using namespace Component;

void ImGuiSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ImGui::SetNextWindowPos(ImVec2(50.0f, 50.0f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300.0f, 200.0f), ImGuiCond_Once);
	ImGui::Begin("ChunkWindow");

	a_chunk.ImGuiInPut();

	a_chunk.ImGuiOutPut();

	ImGui::End();


	ImGui::SetNextWindowPos(ImVec2(50.0f, 600.0f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(200.0f, 200.0f), ImGuiCond_Once);
	ImGui::Begin("Test");

	
	ImGui::End();


}