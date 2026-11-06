#include "ContentBrowserPanel.h"

#include "Engine.h"
#include "Entity.h"
#include "Texture2D.h"
#include "imgui.h"

#include "EditorAssetEntries.h"
#include "WorldService.h"
#include "AnimationAsset.h"
#include "AnimationAssetEditor.h"
#include "ClipAsset.h"
#include "EditorState.h"
#include "InspectorPropertyDrawer.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>


namespace buki
{
	bool CreateAnimationAsset(
		const std::string& directory,
		const std::string& baseName)
	{
		std::string name = baseName.empty() ? "NewAnimation" : baseName;
		std::string path = directory + "/" + name + ".anim";

		int suffix = 1;

		while (std::ifstream(path).good())
		{
			path = directory + "/" + name + std::to_string(suffix++) + ".anim";
		}

		AnimationAsset asset;
		asset.name = name;
		asset.initialState = "Idle";

		AnimationState idleState;
		idleState.id = GenerateEditorId();
		idleState.name = "Idle";
		idleState.clipPath = "";

		asset.states.push_back(idleState);

		bool result = asset.SaveToFile(path);
		return result;
	}

	bool CreateClipAsset(
		const std::string& directory,
		const std::string& baseName)
	{
		std::string name = baseName.empty() ? "NewClip" : baseName;
		std::string path = directory + "/" + name + ".clip";

		int suffix = 1;

		while (std::ifstream(path).good())
		{
			path = directory + "/" + name + std::to_string(suffix++) + ".clip";
		}

		ClipAsset clip;
		clip.name = name;
		clip.loop = true;

		SpriteFrame frame;
		frame.texturePath = "";
		frame.duration = 0.1f;

		clip.frames.push_back(frame);

		return clip.SaveToFile(path);
	}
}

void buki::ContentBrowserPanel::Render(EditorState& state)
{
	ImGui::Begin("Content Browser", &state.showContentBrowser);

	auto* world = Engine::Get().GetWorldPtr();

	ImGui::TextWrapped("Path: %s", state.currentContentPath.string().c_str());
	ImGui::Separator();

	if (state.currentContentPath.has_parent_path())
	{
		if (ImGui::Button("Up"))
			state.currentContentPath = state.currentContentPath.parent_path();
	}

	ImGui::SameLine();
	if (ImGui::Button("Scenes Root"))
		state.currentContentPath = state.scenesRootPath;

	ImGui::SameLine();
	if (ImGui::Button("Prefabs Root"))
		state.currentContentPath = fs::path("../Deployment/Prefabs");

	ImGui::SameLine();
	if (ImGui::Button("Assets Root"))
		state.currentContentPath = fs::path("../Deployment/assets");

	ImGui::SameLine();
	const bool canSaveScene = (world != nullptr && !world->GetCurrentScenePath().empty());
	if (!canSaveScene)


		ImGui::BeginDisabled();

	if (ImGui::Button("Save Scene"))
	{
		if (world != nullptr && world->SaveCurrentScene())
			state.ClearSceneDirty();
	}

	if (!canSaveScene)
		ImGui::EndDisabled();

	ImGui::Spacing();

	if (!fs::exists(state.currentContentPath) || !fs::is_directory(state.currentContentPath))
	{
		ImGui::TextUnformatted("Current path is not a valid directory.");
		ImGui::End();
		return;
	}


	std::vector<BrowserEntry> entries = CollectBrowserEntries(state.currentContentPath, false);

	const float thumbnailSize = 128.0f;
	const float padding = thumbnailSize / 8.0f;
	const float cellSize = thumbnailSize + padding;
	const float panelWidth = ImGui::GetContentRegionAvail().x;

	int columnCount = static_cast<int>(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;

	ImGui::Columns(columnCount, nullptr, false);

	const bool browserHovered = ImGui::IsWindowHovered();
	const bool browserDoubleClicked = browserHovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);

	if (browserHovered && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		ImGui::OpenPopup("browserRightClick");

	if (ImGui::BeginPopup("browserRightClick"))
	{
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Animation"))
			{
				CreateAnimationAsset(
					state.currentContentPath.string(),
					"NewAnimation");
				state.assetDatabase.ScanDirectory("../Deployment");
				InspectorPropertyDrawer::RefreshAssetPickers();
			}

			if (ImGui::MenuItem("Animation Clip"))
			{
				CreateClipAsset(
					state.currentContentPath.string(),
					"NewClip");
				state.assetDatabase.ScanDirectory("../Deployment");
				InspectorPropertyDrawer::RefreshAssetPickers();
			}

			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}


	for (const BrowserEntry& item : entries)
	{
		const bool selected = state.selectedItem == item.path;

		const bool clicked = DrawBrowserTile(item, thumbnailSize, selected);
		const bool hovered = ImGui::IsItemHovered();
		const bool doubleClicked = hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
		const bool rightClicked = hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Right);

		if (clicked)
		{
			OnClick(state, item);
		}

		if (doubleClicked)
		{
			OnDoubleClick(state, item);
		}

		if (rightClicked)
		{
			ImGui::OpenPopup(item.path.c_str());
		}
		if (ImGui::BeginPopup(item.path.c_str()))
		{
			OnRightClick(state, item);
			ImGui::EndPopup();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PREFAB"))
			{
				const char* dropped = static_cast<const char*>(payload->Data);
				if (dropped != nullptr && world != nullptr)
					world->InstantiatePrefab(fs::path(dropped).string());
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::NextColumn();
	}

	if (!state.renamePath.empty() &&
		!ImGui::IsPopupOpen("Rename Asset"))
	{
		ImGui::OpenPopup("Rename Asset");
	}

	if (ImGui::BeginPopupModal("Rename Asset"))
	{
		char buffer[256];
		std::snprintf(
			buffer,
			sizeof(buffer),
			"%s",
			state.renameBuffer.c_str());

		ImGui::SetKeyboardFocusHere();
		const bool submitted = ImGui::InputText(
			"##Rename",
			buffer,
			sizeof(buffer),
			ImGuiInputTextFlags_EnterReturnsTrue);

		if (submitted || ImGui::Button("Rename"))
		{
			fs::path newName(buffer);

			if (newName.extension().empty())
			{
				newName.replace_extension(
					state.renamePath.extension());
			}

			const fs::path parent =
				state.renamePath.parent_path();

			const std::string newPath =
				(parent / newName).string();

			if (state.assetDatabase.RenameAsset(
				state.renamePath.string(),
				newPath))
			{
				state.renamePath.clear();
				state.renameBuffer.clear();
				ImGui::CloseCurrentPopup();
			}
			else
			{
				ImGui::TextColored(
					ImVec4(1.0f, 0.4f, 0.4f, 1.0f),
					"Rename failed.");
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			state.renamePath.clear();
			state.renameBuffer.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (!state.deleteCandidate.empty() &&
		!ImGui::IsPopupOpen("Confirm Delete"))
	{
		ImGui::OpenPopup("Confirm Delete");
	}

	if (ImGui::BeginPopupModal("Confirm Delete"))
	{
		ImGui::Text(
			"Delete %s?",
			state.deleteCandidate.filename().string().c_str());

		ImGui::TextDisabled(
			"Animations referencing this clip will have their reference cleared.");

		if (ImGui::Button("Delete"))
		{
			state.assetDatabase.DeleteAsset(
				state.deleteCandidate.string());

			state.deleteCandidate.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			state.deleteCandidate.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::Columns(1);
	ImGui::End();
}