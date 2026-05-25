#include "ContentBrowserPanel.h"

#include "Engine.h"
#include "Entity.h"
#include "Texture2D.h"
#include "imgui.h"

#include "EditorAssetEntries.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <vector>

namespace
{

	void ClearSceneSelection(buki::EditorState& state)
	{
		state.selectedEntity = nullptr;
		state.activeEntity = nullptr;
		state.selectedEntities.clear();
	}

	ImTextureID ToImGuiTextureID(std::uint32_t textureId)
	{
		return static_cast<ImTextureID>(textureId);
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
			state.sceneDirty = false;
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


	std::vector<BrowserEntry> entries = buki::CollectBrowserEntries(state.currentContentPath, false);

	std::sort(entries.begin(), entries.end(),
		[](const BrowserEntry& a, const BrowserEntry& b)
		{
			if (a.isDirectory != b.isDirectory)
				return a.isDirectory > b.isDirectory;
			return ToLowerCopy(a.displayName) < ToLowerCopy(b.displayName);
		});

	const float thumbnailSize = 128.0f;
	const float padding = thumbnailSize / 8.0f;
	const float cellSize = thumbnailSize + padding;
	const float panelWidth = ImGui::GetContentRegionAvail().x;

	int columnCount = static_cast<int>(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;

	ImGui::Columns(columnCount, nullptr, false);

	for (const BrowserEntry& item : entries)
	{
		const bool selected = (item.isSceneFile && state.selectedScenePath == item.fullPath);
		const bool clicked = DrawBrowserTile(item, thumbnailSize, selected);
		const bool doubleClicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);

		if (clicked)
		{
			if (item.isDirectory)
			{
				state.currentContentPath = item.fullPath;
			}
			else if (item.isSceneFile)
			{
				state.selectedScenePath = item.fullPath;
			}
		}

		if (doubleClicked)
		{
			if (item.isDirectory)
			{
				state.currentContentPath = item.fullPath;
			}
			else if (item.isSceneFile)
			{
				if (world != nullptr && world->LoadScene(item.fullPath.string()))
				{
					ClearSceneSelection(state);
					state.sceneDirty = false;

					for (auto entity : world->GetEntitiesInWorld())
					{
						if (entity != nullptr)
							entity->Set();
					}
				}
			}
			else if (item.isPrefabFile)
			{
				if (ImGui::Selectable(item.displayName.c_str(), state.selectedPrefabPath == item.fullPath, ImGuiSelectableFlags_AllowDoubleClick))
				{
					state.selectedPrefabPath = item.fullPath;
					state.selectedScenePath.clear();
					state.selectedEntity = nullptr;
					state.selectedEntities.clear();
					state.activeEntity = nullptr;

					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						state.previewPrefabPath = item.fullPath;
						state.showPrefabPreview = true;
					}
				}

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					const std::string pathStr = item.fullPath.string();
					ImGui::SetDragDropPayload("PREFAB", pathStr.c_str(), pathStr.size() + 1);
					ImGui::TextUnformatted(pathStr.c_str());
					ImGui::EndDragDropSource();
				}

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Instantiate"))
					{
						if (world != nullptr)
							world->InstantiatePrefab(item.fullPath.string());
					}
					if (ImGui::MenuItem("Preview"))
					{
						state.previewPrefabPath = item.fullPath;
						state.showPrefabPreview = true;
					}
					ImGui::EndPopup();
				}
			}
		}

		ImGui::NextColumn();
	}

	ImGui::Columns(1);

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

	ImGui::End();
}