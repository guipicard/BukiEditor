#include "ContentBrowserPanel.h"

#include "Engine.h"
#include "Entity.h"
#include "Texture2D.h"
#include "imgui.h"

#include "EditorAssetEntries.h"
#include "WorldService.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>

namespace
{

	void ClearSceneSelection(buki::EditorState& state)
	{
		state.selectedEntity = nullptr;
		state.activeEntity = nullptr;
		state.selectedEntities.clear();
	}

	void ClearPrefabSelection(buki::EditorState& state)
	{
		state.selectedPrefabEntity = nullptr;
		state.selectedPrefabPath.clear();
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
		const bool selected =
			(item.isSceneFile && state.selectedScenePath == item.fullPath) ||
			(item.isPrefabFile && state.selectedPrefabPath == item.fullPath);

		const bool clicked = DrawBrowserTile(item, thumbnailSize, selected);
		const bool hovered = ImGui::IsItemHovered();
		const bool doubleClicked = hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);

		if (clicked)
		{
			if (item.isDirectory)
			{
				state.currentContentPath = item.fullPath;
			}
			else if (item.isSceneFile)
			{
				state.selectedScenePath = item.fullPath;
				state.selectedPrefabPath.clear();
				state.selectedPrefabEntity = nullptr;
			}
			else if (item.isPrefabFile)
			{
				SelectPrefab(item.fullPath, state);
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
				state.selectedScenePath = item.fullPath.string();
				state.requestSceneWindowFocus = true;
				if (world != nullptr && world->LoadScene(item.fullPath.string()))
				{
					ClearSceneSelection(state);
					state.activePrefabPreviewIndex = -1;
					state.selectedPrefabPath.clear();
					state.selectedPrefabEntity = nullptr;
					state.sceneDirty = false;
					state.scenePreviewSession.cameraSettings.Deserialize(item.fullPath);
					state.scenePreviewSession.path = item.fullPath;
					state.selectedScenePath = item.fullPath;
					Vector2 camPos = state.scenePreviewSession.cameraSettings.position;
					Engine::Get().GetActiveCameraPtr()->position = { camPos.x, camPos.y };

					for (auto entity : world->GetEntitiesInWorld())
					{
						if (entity != nullptr)
							entity->Set();
					}
				}
			}
			else if (item.isPrefabFile)
			{
				SelectPrefab(item.fullPath, state);
				OpenPrefabPreview(item.fullPath, state);
				state.prefabPreviewSessions[state.activePrefabPreviewIndex].cameraSettings.Deserialize(item.fullPath);
				Vector2 camPos = state.prefabPreviewSessions[state.activePrefabPreviewIndex].cameraSettings.position;
				Engine::Get().GetActiveCameraPtr()->position = { camPos.x, camPos.y };
			}
		}

		if (item.isPrefabFile)
		{

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				const std::string pathStr = item.fullPath.string();
				ImGui::SetDragDropPayload("PREFAB", pathStr.c_str(), pathStr.size() + 1);
				ImGui::TextUnformatted(item.displayName.c_str());
				ImGui::EndDragDropSource();
			}

			if (hovered && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
				ImGui::OpenPopup(item.fullPath.string().c_str());

			if (ImGui::BeginPopup(item.fullPath.string().c_str()))
			{
				if (ImGui::MenuItem("Instantiate"))
				{
					if (world != nullptr)
						world->InstantiatePrefab(item.fullPath.string());
				}

				if (ImGui::MenuItem("Preview"))
				{
					SelectPrefab(item.fullPath, state);
					OpenPrefabPreview(item.fullPath, state);
				}

				ImGui::EndPopup();
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

void buki::ContentBrowserPanel::SelectPrefab(const std::filesystem::path& path, EditorState& state)
{
	state.selectedEntity = nullptr;
	state.activeEntity = nullptr;
	state.selectedEntities.clear();

	state.selectedPrefabPath = fs::absolute(path).lexically_normal();

	IWorld* world = Engine::Get().GetWorldPtr();
	state.selectedPrefabEntity = (world != nullptr)
		? world->GetOrLoadPrefabEntity(state.selectedPrefabPath)
		: nullptr;
}

bool buki::ContentBrowserPanel::OpenPrefabPreview(const std::filesystem::path& path, EditorState& state)
{
	IWorld* world = Engine::Get().GetWorldPtr();
	if (world == nullptr)
	{
		return false;
	}

	const auto normalized = fs::absolute(path).lexically_normal();

	if (state.activePrefabPreviewIndex >= 0 &&
		state.activePrefabPreviewIndex < static_cast<int>(state.prefabPreviewSessions.size()))
	{
		state.prefabPreviewSessions[state.activePrefabPreviewIndex].focused = false;
	}

	for (size_t i = 0; i < state.prefabPreviewSessions.size(); ++i)
	{
		PrefabPreviewSession& session = state.prefabPreviewSessions[i];
		if (session.path == normalized)
		{
			session.open = true;
			session.focused = true;
			session.requestFocus = true;
			session.prefabEntity = world->GetOrLoadPrefabEntity(normalized);

			state.activePrefabPreviewIndex = static_cast<int>(i);
			state.selectedPrefabPath = normalized;
			state.selectedPrefabEntity = session.prefabEntity;
			return session.prefabEntity != nullptr;
		}
	}

	PrefabPreviewSession session;
	session.path = normalized;
	session.prefabEntity = world->GetOrLoadPrefabEntity(normalized);
	session.open = (session.prefabEntity != nullptr);
	session.focused = session.open;
	session.requestFocus = session.open;
	session.requestDockNextToScene = session.open;
	session.windowId = normalized.string();

	if (!session.open)
	{
		return false;
	}

	state.prefabPreviewSessions.push_back(session);
	state.activePrefabPreviewIndex = static_cast<int>(state.prefabPreviewSessions.size() - 1);
	state.selectedPrefabPath = normalized;
	state.selectedPrefabEntity = session.prefabEntity;
	return true;
}