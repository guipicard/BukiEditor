#include "SceneViewPanel.h"

#include "imgui.h"

void buki::SceneViewPanel::Render(EditorState& state)
{
	ImGui::Begin("Scene View", &state.showSceneView);

	state.viewportFocused = ImGui::IsWindowFocused();
	state.viewportHovered = ImGui::IsWindowHovered();

	const ImVec2 avail = ImGui::GetContentRegionAvail();
	state.viewportWidth = avail.x;
	state.viewportHeight = avail.y;

	ImGui::Text("Viewport: %.0f x %.0f", state.viewportWidth, state.viewportHeight);
	ImGui::Text("Focused: %s", state.viewportFocused ? "true" : "false");
	ImGui::Text("Hovered: %s", state.viewportHovered ? "true" : "false");
	ImGui::Separator();

	ImGui::InvisibleButton("SceneViewportSurface", avail);

	const ImVec2 min = ImGui::GetItemRectMin();
	const ImVec2 max = ImGui::GetItemRectMax();
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	drawList->AddRectFilled(min, max, IM_COL32(35, 35, 40, 255));
	drawList->AddRect(min, max, IM_COL32(90, 90, 110, 255));
	drawList->AddText(ImVec2(min.x + 12.0f, min.y + 12.0f), IM_COL32(220, 220, 220, 255), "Future engine viewport");

	ImGui::End();
}