#include "ContentBrowserPanel.h"

#include "Engine.h"
#include "imgui.h"

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

void buki::ContentBrowserPanel::Render(EditorState& state)
{
	ImGui::Begin("Content Browser", &state.showContentBrowser);

	ImGui::TextWrapped("Path: %s", state.currentContentPath.string().c_str());
	ImGui::Separator();

	if (state.currentContentPath.has_parent_path())
	{
		if (ImGui::Button("Up"))
		{
			state.currentContentPath = state.currentContentPath.parent_path();
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Scenes Root"))
	{
		state.currentContentPath = state.scenesRootPath;
	}

	ImGui::SameLine();

	auto* world = buki::Engine::Get().GetWorldPtr();
	const bool canSaveLoadedScene = (world != nullptr && world->HasCurrentScenePath());

	if (!canSaveLoadedScene)
	{
		ImGui::BeginDisabled();
	}

	if (ImGui::Button("Save Loaded Scene"))
	{
		if (world != nullptr && world->SaveCurrentScene())
		{
			state.sceneDirty = false;
			state.selectedScenePath = std::filesystem::path(world->GetCurrentScenePath());
		}
	}

	if (!canSaveLoadedScene)
	{
		ImGui::EndDisabled();
	}

	ImGui::Spacing();

	if (!fs::exists(state.currentContentPath))
	{
		ImGui::TextUnformatted("Current path does not exist.");
		ImGui::End();
		return;
	}

	if (!fs::is_directory(state.currentContentPath))
	{
		ImGui::TextUnformatted("Current path is not a directory.");
		ImGui::End();
		return;
	}

	for (const auto& entry : fs::directory_iterator(state.currentContentPath))
	{
		const fs::path& path = entry.path();
		const std::string name = path.filename().string();

		if (entry.is_directory())
		{
			if (ImGui::Selectable((name + "/").c_str(), false))
			{
				state.currentContentPath = path;
			}
			continue;
		}

		const std::string ext = path.extension().string();
		const bool isSceneFile = (ext == ".json" || ext == ".scene");
		const bool isSelected = (state.selectedScenePath == path);

		if (isSceneFile)
		{
			if (ImGui::Selectable(name.c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick))
			{
				state.selectedScenePath = path;
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (world != nullptr && world->LoadScene(path.string()))
				{
					state.selectedScenePath = path;
					state.selectedEntity = nullptr;
					state.sceneDirty = false;
				}
			}
		}
		else
		{
			ImGui::TextUnformatted(name.c_str());
		}
	}

	ImGui::End();
}
