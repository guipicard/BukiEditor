#include "SceneViewPanel.h"

#include "imgui.h"

void buki::SceneViewPanel::Render(EditorState& state)
{
	ImGui::Begin("Scene", &state.showSceneView);

	state.viewportFocused = ImGui::IsWindowFocused();
	state.viewportHovered = ImGui::IsWindowHovered();

	const ImVec2 avail = ImGui::GetContentRegionAvail();
	state.viewportWidth = avail.x;
	state.viewportHeight = avail.y;

	ImGui::Text("Viewport: %.0f x %.0f", state.viewportWidth, state.viewportHeight);
	ImGui::Text("Focused: %s", state.viewportFocused ? "true" : "false");
	ImGui::Text("Hovered: %s", state.viewportHovered ? "true" : "false");

	if (state.selectedScenePath.empty())
	{
		ImGui::TextUnformatted("Selected scene: <none>");
	}
	else
	{
		ImGui::Text("Selected scene: %s", state.selectedScenePath.filename().string().c_str());
	}

	ImGui::Separator();
	ImGui::TextWrapped("Viewport texture is not wired yet. Next step is rendering the game scene to an OpenGL framebuffer and showing it here with ImGui::Image().");

	ImGui::Dummy(ImVec2(avail.x, avail.y - 80.0f));

	ImGui::End();
}