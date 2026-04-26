#include "ContentBrowserPanel.h"

#include "imgui.h"

void buki::ContentBrowserPanel::Render(EditorState& state)
{
	ImGui::Begin("Content Browser", &state.showContentBrowser);

	ImGui::Text("Current Path: %s", state.currentContentPath.string().c_str());
	ImGui::Separator();

	ImGui::BulletText("Assets/");
	ImGui::BulletText("Textures/");
	ImGui::BulletText("Scenes/");
	ImGui::BulletText("Audio/");
	ImGui::BulletText("Scripts/");

	ImGui::End();
}