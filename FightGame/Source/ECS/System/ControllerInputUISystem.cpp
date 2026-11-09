#include "ControllerInputUISystem.h"
#include "Components.h"

using namespace Component;

void ControllerInputUISystem(Chunk& a_chunk, const SystemContext& a_context)
{
	ComponentView view = a_chunk.GetView<ComponentTypes<ControllerInputUIReplace, UIComponent>>();

	const bool isConnected = a_context.input.IsUseController();

	for (auto it : view)
	{
		ComponentHandle<ControllerInputUIReplace> replaceUI = a_chunk.GetComponent<ControllerInputUIReplace>(it);
		if (!replaceUI.IsValid()) continue;

		ComponentHandle<UIComponent> ui = a_chunk.GetComponent<UIComponent>(it);
		if (!ui.IsValid()) continue;

		const std::string& targetKey = isConnected
			? replaceUI.Look().controllerUIKey
			: replaceUI.Look().keyboardUIKey;

		const float2& targetScale = isConnected
			? replaceUI.Look().controllerUIScale
			: replaceUI.Look().keyboardUIScale;

		const bool hasTargetKey = !targetKey.empty();
		const bool hasTargetScale = targetScale.x != 0.0f || targetScale.y != 0.0f;
		if (!hasTargetKey && !hasTargetScale) continue;

		const bool keyChanged = hasTargetKey && ui.Look().key != targetKey;
		const bool scaleChanged = hasTargetScale
			&& (ui.Look().uiScale.x != targetScale.x || ui.Look().uiScale.y != targetScale.y);

		if (!keyChanged && !scaleChanged) continue;

		if (keyChanged)
		{
			ui->key = targetKey;
		}
		if (scaleChanged)
		{
			ui->uiScale = targetScale;
		}
	}
}
