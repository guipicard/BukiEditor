#include "HierarchyPanel.h"

#include "imgui.h"

void buki::HierarchyPanel::Render(EditorState& state)
{
	ImGui::Begin("Hierarchy", &state.showHierarchy);

	ImGui::TextUnformatted("Scene");
	ImGui::Separator();

	if (ImGui::Selectable("Camera", state.selectedEntityId == 0))
		state.selectedEntityId = 0;

	if (ImGui::Selectable("Player", state.selectedEntityId == 1))
		state.selectedEntityId = 1;

	if (ImGui::Selectable("Enemy", state.selectedEntityId == 2))
		state.selectedEntityId = 2;

	if (ImGui::Selectable("Light", state.selectedEntityId == 3))
		state.selectedEntityId = 3;

	ImGui::End();
}