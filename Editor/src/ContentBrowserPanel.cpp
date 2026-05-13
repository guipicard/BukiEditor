#include "ContentBrowserPanel.h"

#include "Engine.h"
#include "Entity.h"
#include "imgui.h"
#include <string>
#include <algorithm>

#include <fstream>

namespace fs = std::filesystem;

namespace
{
	bool IsImageExtension(const fs::path& path)
	{
		std::string ext = path.extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });

		return ext == ".png" ||
			ext == ".jpg" ||
			ext == ".jpeg" ||
			ext == ".bmp" ||
			ext == ".tga" ||
			ext == ".gif" ||
			ext == ".webp";
	}

	std::string ToAssetRelativePath(const fs::path& fullPath)
	{
		std::string normalized = fullPath.lexically_normal().generic_string();
		const std::string marker = "/assets/";
		size_t pos = normalized.find(marker);
		if (pos != std::string::npos)
		{
			return normalized.substr(pos);
		}
		return normalized;
	}
}

void buki::ContentBrowserPanel::Render(EditorState& state)
{
	ImGui::Begin("Content Browser", &state.showContentBrowser);

	auto* world = buki::Engine::Get().GetWorldPtr();

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
	bool canSaveScene = (world != nullptr && !world->GetCurrentScenePath().empty());
	if (!canSaveScene)
		ImGui::BeginDisabled();

	if (ImGui::Button("Save Scene"))
	{
		if (world != nullptr)
		{
			if (world->SaveCurrentScene())
			{
				state.sceneDirty = false;
			}
		}
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

	for (const auto& entry : fs::directory_iterator(state.currentContentPath))
	{
		const fs::path& path = entry.path();
		const std::string name = path.filename().string();

		if (entry.is_directory())
		{
			if (ImGui::Selectable((name + "/").c_str(), false))
				state.currentContentPath = path;
			continue;
		}

		const std::string ext = path.extension().string();
		const bool isSceneFile = (ext == ".json" || ext == ".scene");
		const bool isPrefabFile = (ext == ".prefab");
		const bool isImageFile = IsImageExtension(path);

		if (isSceneFile)
		{
			if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
			{
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (world != nullptr && world->LoadScene(path.string()))
					{
						state.selectedScenePath = path;
						state.selectedEntity = nullptr;
						state.sceneDirty = false;

						for (auto entity : world->GetEntitiesInWorld())
						{
							entity->Set();
						}
					}
				}
				else
				{
					state.selectedScenePath = path;
				}
			}
		}
		else if (isPrefabFile)
		{
			if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
			{
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					world->InstantiatePrefab(path.string());
				}
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				const std::string pathStr = path.string();
				ImGui::SetDragDropPayload("PREFAB", pathStr.c_str(), pathStr.size() + 1);
				ImGui::TextUnformatted(name.c_str());
				ImGui::EndDragDropSource();
			}
		}
		else if (isImageFile)
		{
			if (ImGui::Selectable(name.c_str(), false))
			{
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				const std::string relativePath = ToAssetRelativePath(path);
				ImGui::SetDragDropPayload("IMAGE", relativePath.c_str(), relativePath.size() + 1);
				ImGui::TextUnformatted(name.c_str());
				ImGui::TextDisabled("%s", relativePath.c_str());
				ImGui::EndDragDropSource();
			}
		}
		else
		{
			ImGui::TextUnformatted(name.c_str());
		}
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PREFAB"))
		{
			const char* dropped = static_cast<const char*>(payload->Data);
			if (dropped != nullptr)
			{
				world->InstantiatePrefab(fs::path(dropped).string());
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::End();
}