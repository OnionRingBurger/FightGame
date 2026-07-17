#include "ImGuiSystem.h"
#include "Components.h"
#include "imGui/imgui.h"

using namespace Component;

void ImGuiSystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ImGui::SetNextWindowPos(ImVec2(50.0f, 50.0f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300.0f, 200.0f), ImGuiCond_Once);
	ImGui::Begin("DefaultWindow");


	a_chunk.ImGuiInPut();

	a_chunk.ImGuiOutPut();



	float lx = a_context.input.GetLeftAxis().x;
	float ly = a_context.input.GetLeftAxis().y;
	float rx = a_context.input.GetRightAxis().x;
	float ry = a_context.input.GetRightAxis().y;

	static float3 CamerSlider = { 0.0f, 0.0f, 0.0f };
	//ImGui::SliderFloat("x", &CamerSlider.x, 180.0f, -180.0f);
	//ImGui::SliderFloat("y", &CamerSlider.y, 180.0f, -180.0f);
	//ImGui::SliderFloat("z", &CamerSlider.z, 180.0f, -180.0f);
	ComponentView view = a_chunk.GetView<ComponentTypes<Position, Rotation, Camera>>();
	for (auto it : view)
	{
		const ComponentHandle<Position> position = a_chunk.GetComponent<Position>(it);
		const ComponentHandle<Rotation> rotation = a_chunk.GetComponent<Rotation>(it);

		ImGui::Text("pos.x,%.2f", position.Look().x);
		ImGui::Text("pos.y,%.2f", position.Look().y);
		ImGui::Text("pos.z,%.2f", position.Look().z);

		ImGui::Text("pitch,%.2f", rotation.Look().pitch);
		ImGui::Text("yaw,%.2f", rotation.Look().yaw);
		ImGui::Text("roll,%.2f", rotation.Look().roll);

		//rotation->x = CamerSlider.x;
		//rotation->y = CamerSlider.y;
		//rotation->z = CamerSlider.z;
	}

	ImGui::Text("delta,%.2f", a_context.deltaTime);

	ImGui::End();
}