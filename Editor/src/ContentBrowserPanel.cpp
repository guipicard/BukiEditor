#pragma once
#include "ContentBrowserPanel.h"

#include "Texture2D.h"
#include "imgui.h"

#include "EditorAssetEntries.h"
#include "WorldService.h"
#include "AnimationAsset.h"
#include "ClipAsset.h"
#include "InspectorPropertyDrawer.h"
#include "IAssetBehavior.h"
#include "EditorState.h"
#include "EditorAssetDatabase.h"
#include "AssetType.h"
#include "AnimationStateMachineEditorWindow.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <vector>
#include <fstream>

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

	const bool browserHovered = ImGui::IsWindowHovered();
	const bool browserDoubleClicked = browserHovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);

	if (browserHovered && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		ImGui::OpenPopup("browserRightClick");

	if (ImGui::BeginPopup("browserRightClick"))
	{
		if (ImGui::BeginMenu("Create"))
		{
			const auto createAsset = [&](AssetType type, const char* baseName)
				{
					IAssetBehavior* behavior = state.assetBehaviors.Find(type);

					if (behavior == nullptr)
					{
						Engine::Get().Log().LogWarning("No asset behavior is registered for the requested asset type.");
						return;
					}

					std::string createdAssetPath;

					if (!behavior->Create(state.currentContentPath.string(), baseName, createdAssetPath))
					{
						Engine::Get().Log().LogWarning("Failed to create asset: " + std::string(baseName));
						return;
					}

					state.assetDatabase.ScanDirectory("../Deployment");
					InspectorPropertyDrawer::RefreshAssetPickers();
				};

			if (ImGui::MenuItem("Scene"))
			{
				createAsset(AssetType::Scene, "New Scene");
			}

			if (ImGui::MenuItem("Animation"))
			{
				createAsset(AssetType::Anim, "New Animator");
			}

			if (ImGui::MenuItem("Animation Clip"))
			{
				createAsset(AssetType::Clip, "New Clip");
			}

			ImGui::EndMenu();
		}

		ImGui::EndPopup();
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
		ImGui::NextColumn();
	}

	if (state.renamePopupOpen)
	{
		state.renamePopupOpen = false;
		ImGui::OpenPopup("Rename Asset");
	}

	if (ImGui::BeginPopupModal("Rename Asset", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		char buffer[256];
		std::snprintf(buffer, sizeof(buffer), "%s", state.renameBuffer.c_str());

		if (ImGui::IsWindowAppearing())
		{
			ImGui::SetKeyboardFocusHere();
		}

		const bool submitted = ImGui::InputText("##Rename", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue);

		state.renameBuffer = buffer;

		const bool cancelled = ImGui::IsKeyPressed(ImGuiKey_Escape);

		if (submitted || ImGui::Button("Rename"))
		{
			fs::path newName(buffer);

			if (newName.extension().empty())
			{
				newName.replace_extension(state.renamePath.extension());
			}

			const fs::path parent = state.renamePath.parent_path();

			const std::string oldPath = state.renamePath.string();

			const std::string newPath = (parent / newName).string();

			if (state.assetDatabase.RenameAsset(oldPath, newPath))
			{
				const std::vector<std::string>& changedAssets = state.assetDatabase.GetLastChangedReferencingAssets();

				// The renamed asset itself may be open.
				if (AssetEditorWindow* renamedWindow = state.assetEditorWindows.FindWindow(oldPath))
				{
					renamedWindow->Close();
				}

				// Reopen the renamed asset if it was previously open.
				state.assetEditorWindows.OpenAssetWindow<AnimationStateMachineEditorWindow>(newPath, state, newPath);

				// Reload every open asset that referenced the renamed asset.
				for (const std::string& changedPath : changedAssets)
				{
					if (AssetEditorWindow* referencingWindow = state.assetEditorWindows.FindWindow(changedPath))
					{
						referencingWindow->Reload();
					}
				}

				InspectorPropertyDrawer::RefreshAssetPickers();
			}
			else
			{
				ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Rename failed.");
			}
			state.renamePopupOpen = false;
			state.renamePath.clear();
			state.renameBuffer.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (cancelled || ImGui::Button("Cancel"))
		{
			state.renamePath.clear();
			state.renameBuffer.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (state.deletePopupOpen)
	{
		state.deletePopupOpen = false;
		ImGui::OpenPopup("Confirm Delete");
	}

	if (ImGui::BeginPopupModal("Confirm Delete"))
	{
		ImGui::Text("Delete %s?", state.deleteCandidate.filename().string().c_str());

		ImGui::TextDisabled("Animations referencing this clip will have their reference cleared.");

		if (ImGui::Button("Delete"))
		{
			const std::string deletedPath =
				state.deleteCandidate.string();

			state.assetDatabase.DeleteAsset(deletedPath);

			const std::vector<std::string>& changedAssets =
				state.assetDatabase.GetLastChangedReferencingAssets();

			// Close the deleted asset if it is open.
			if (AssetEditorWindow* deletedWindow =
				state.assetEditorWindows.FindWindow(deletedPath))
			{
				deletedWindow->Close();
			}

			// Reload every open asset that referenced the deleted asset.
			for (const std::string& changedPath : changedAssets)
			{
				if (AssetEditorWindow* referencingWindow =
					state.assetEditorWindows.FindWindow(changedPath))
				{
					referencingWindow->Reload();
				}
			}

			state.deleteCandidate.clear();
			InspectorPropertyDrawer::RefreshAssetPickers();
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