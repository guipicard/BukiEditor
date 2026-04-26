#include "InspectorPanel.h"

#include "imgui.h"

void buki::InspectorPanel::Render(EditorState& state)
{
	ImGui::Begin("Inspector", &state.showInspector);

	ImGui::Text("Selected Entity ID: %d", state.selectedEntityId);
	ImGui::Separator();

	if (state.selectedEntityId < 0)
	{
		ImGui::TextUnformatted("No entity selected.");
		ImGui::End();
		return;
	}

	static float position[2] = { 0.0f, 0.0f };
	static float rotation = 0.0f;
	static float scale[2] = { 1.0f, 1.0f };

	ImGui::TextUnformatted("Transform");
	ImGui::DragFloat2("Position", position, 0.1f);
	ImGui::DragFloat("Rotation", &rotation, 0.1f);
	ImGui::DragFloat2("Scale", scale, 0.1f, 0.0f, 100.0f);

	ImGui::End();
}