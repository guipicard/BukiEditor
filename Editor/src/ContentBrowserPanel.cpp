#include "ContentBrowserPanel.h"

#include "Engine.h"
#include "Entity.h"
#include "Texture2D.h"
#include "imgui.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <vector>

namespace
{
	std::string ToAssetRelativePath(const fs::path& fullPath)
	{
		std::string normalized = fullPath.lexically_normal().generic_string();

		const std::string marker = "/assets/";
		const size_t pos = normalized.find(marker);
		if (pos != std::string::npos)
			return normalized.substr(pos + 1); // remove leading '/'

		const std::string deploymentMarker = "Deployment/";
		const size_t deploymentPos = normalized.find(deploymentMarker);
		if (deploymentPos != std::string::npos)
			return normalized.substr(deploymentPos + deploymentMarker.size());

		return fullPath.lexically_normal().generic_string();
	}

	void ClearSceneSelection(buki::EditorState& state)
	{
		state.selectedEntity = nullptr;
		state.activeEntity = nullptr;
		state.selectedEntities.clear();
	}

	ImTextureID ToImGuiTextureID(std::uint32_t textureId)
	{
		return static_cast<ImTextureID>(textureId);
		// If your ImTextureID is pointer-based instead, use:
		// return (ImTextureID)(intptr_t)textureId;
	}
}

std::string buki::ToLowerCopy(std::string value)
{
	std::transform(value.begin(), value.end(), value.begin(),
		[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return value;
}

bool buki::IsPrefabFile(const fs::path& path)
{
	return ToLowerCopy(path.extension().string()) == ".prefab";
}

bool buki::IsImageFile(const fs::path& path)
{
	const std::string ext = ToLowerCopy(path.extension().string());
	return ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" ||
		ext == ".tga" || ext == ".gif" || ext == ".webp";
}

bool buki::IsAudioFile(const fs::path& path)
{
	const std::string ext = ToLowerCopy(path.extension().string());
	return ext == ".wav" || ext == ".mp3" || ext == ".ogg" || ext == ".flac";
}

bool buki::IsSceneFile(const fs::path& path)
{
	const std::string ext = ToLowerCopy(path.extension().string());
	return ext == ".json" || ext == ".scene";
}

buki::BrowserVisualType buki::GetBrowserVisualType(const fs::path& path, bool isDirectory)
{
	if (isDirectory)
		return BrowserVisualType::Folder;

	if (IsSceneFile(path))
		return BrowserVisualType::Scene;

	if (IsPrefabFile(path))
		return BrowserVisualType::Prefab;

	if (IsAudioFile(path))
		return BrowserVisualType::Audio;

	if (IsImageFile(path))
		return BrowserVisualType::Image;

	return BrowserVisualType::File;
}

buki::EditorBrowserIcons& buki::GetEditorBrowserIcons()
{
	static EditorBrowserIcons icons;
	static bool loaded = false;

	if (!loaded)
	{
		auto& textures = Engine::Get().Textures();

		icons.folder = textures.Load("assets/EditorImg/Folder.png");
		icons.network = textures.Load("assets/EditorImg/Network.png");
		icons.audio = textures.Load("assets/EditorImg/Audio.png");
		icons.file = textures.Load("assets/EditorImg/File.png");

		loaded = true;
	}

	return icons;
}

const buki::Texture2D* buki::TryGetPreviewTexture(const fs::path& path)
{
	if (!IsImageFile(path))
		return nullptr;

	auto& textures = Engine::Get().Textures();

	const std::string fullPath = path.lexically_normal().string();
	const std::string assetRelativePath = ToAssetRelativePath(path);

	Texture2D* texture = textures.Get(fullPath);
	if (texture == nullptr)
		texture = textures.Get(assetRelativePath);

	if (texture == nullptr)
		texture = textures.Load(assetRelativePath);

	if (texture == nullptr)
		texture = textures.Load(fullPath, false);

	if (texture == nullptr || !texture->IsValid())
		return nullptr;

	return texture;
}

const buki::Texture2D* buki::GetBrowserThumbnail(const fs::path& path, bool isDirectory)
{
	const BrowserVisualType type = GetBrowserVisualType(path, isDirectory);

	if (type == BrowserVisualType::Image)
	{
		if (const Texture2D* preview = TryGetPreviewTexture(path))
			return preview;
	}

	const EditorBrowserIcons& icons = GetEditorBrowserIcons();

	switch (type)
	{
	case BrowserVisualType::Folder: return icons.folder;
	case BrowserVisualType::Scene:  return icons.network;
	case BrowserVisualType::Prefab: return icons.network;
	case BrowserVisualType::Audio:  return icons.audio;
	case BrowserVisualType::File:   return icons.file;
	case BrowserVisualType::Image:  return icons.file;
	default:                        return icons.file;
	}
}

bool buki::DrawBrowserTile(const BrowserEntry& entry, float thumbnailSize, bool selected)
{
	bool activated = false;

	ImGui::PushID(entry.fullPath.string().c_str());
	ImGui::BeginGroup();

	const Texture2D* thumbnail = GetBrowserThumbnail(entry.fullPath, entry.isDirectory);
	ImVec2 buttonSize(thumbnailSize, thumbnailSize);
	ImVec2 imageSize(thumbnailSize, thumbnailSize);
	ImVec2 padding(8.0f, 8.0f);
	imageSize.x -= padding.x;
	imageSize.y -= padding.y;
	if (thumbnail->height != thumbnail->width)
	{
		float aspect = static_cast<float>(thumbnail->width) / static_cast<float>(thumbnail->height);
		if (aspect > 1.0f)
		{
			imageSize.y /= aspect;
			padding.y = (padding.y +thumbnailSize - imageSize.y) * 0.5f;
		}
		else
		{
			imageSize.x *= aspect;
			padding.x = (padding.x + thumbnailSize - imageSize.x) * 0.5f;
		}
	}
	const ImVec4 transparent(0.0f, 0.0f, 0.0f, 0.0f);
	const ImVec4 selectedTint = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	const ImVec4 normalTint = ImVec4(1.0f, 1.0f, 1.0f, 0.96f);

	ImGui::PushStyleColor(ImGuiCol_Button, transparent);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.08f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.14f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, selected ? 2.0f : 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);

	if (selected)
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.35f, 0.65f, 1.0f, 1.0f));
	else
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.10f));

	if (thumbnail != nullptr && thumbnail->IsValid())
	{
		activated = ImGui::ImageButton(
			"##thumb",
			ToImGuiTextureID(thumbnail->id),
			imageSize,
			ImVec2(0, 0),
			ImVec2(1, 1),
			transparent,
			selected ? selectedTint : normalTint);
	}
	else
	{
		activated = ImGui::Button("##thumb_fallback", buttonSize);
	}

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		if (!entry.payloadType.empty())
		{
			const std::string pathStr = entry.fullPath.string();
			ImGui::SetDragDropPayload(entry.payloadType.c_str(), pathStr.c_str(), pathStr.size() + 1);
			ImGui::TextUnformatted(entry.displayName.c_str());
		}
		ImGui::EndDragDropSource();
	}

	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar(2);

	ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + thumbnailSize);
	ImGui::TextWrapped("%s", entry.displayName.c_str());
	ImGui::PopTextWrapPos();

	ImGui::EndGroup();
	ImGui::PopID();

	return activated;
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

	std::vector<BrowserEntry> entries;

	for (const auto& dirEntry : fs::directory_iterator(state.currentContentPath))
	{
		BrowserEntry item;
		item.fullPath = fs::absolute(dirEntry.path()).lexically_normal();
		item.displayName = item.fullPath.filename().string();
		item.isDirectory = dirEntry.is_directory();
		item.isSceneFile = !item.isDirectory && IsSceneFile(item.fullPath);
		item.isPrefabFile = !item.isDirectory && IsPrefabFile(item.fullPath);
		item.isImageFile = !item.isDirectory && IsImageFile(item.fullPath);
		item.isAudioFile = !item.isDirectory && IsAudioFile(item.fullPath);

		if (item.isPrefabFile) item.payloadType = "PREFAB";
		else if (item.isImageFile) item.payloadType = "IMAGE";
		else if (item.isAudioFile) item.payloadType = "AUDIO";
		else if (!item.isDirectory && item.isSceneFile) item.payloadType = "SCENE";

		entries.push_back(std::move(item));
	}

	std::sort(entries.begin(), entries.end(),
		[](const BrowserEntry& a, const BrowserEntry& b)
		{
			if (a.isDirectory != b.isDirectory)
				return a.isDirectory > b.isDirectory;
			return ToLowerCopy(a.displayName) < ToLowerCopy(b.displayName);
		});

	//const float padding = 0.0f;
	const float padding = 16.0f;
	const float thumbnailSize = 128.0f;
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
				if (world != nullptr)
					world->InstantiatePrefab(item.fullPath.string());
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