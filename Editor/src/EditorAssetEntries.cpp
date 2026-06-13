#pragma once
#include "EditorAssetEntries.h"

#include "Engine.h"
#include "Texture2D.h"
#include "EditorState.h"
#include "EditorViewportHelpers.h"

//#include <algorithm>
//#include <cctype>
//#include <cstdint>
//#include <cstring>

namespace
{
	bool MatchesPayloadType(const buki::BrowserEntry& entry, const char* payloadType)
	{
		if (payloadType == nullptr || payloadType[0] == '\0')
			return true;

		if (std::strcmp(payloadType, "IMAGE") == 0)
			return entry.isImageFile;

		if (std::strcmp(payloadType, "AUDIO") == 0)
			return entry.isAudioFile;

		if (std::strcmp(payloadType, "PREFAB") == 0)
			return entry.isPrefabFile;

		if (std::strcmp(payloadType, "SCENE") == 0)
			return entry.isSceneFile;

		return false;
	}

	buki::BrowserEntry MakeBrowserEntry(const fs::directory_entry& dirEntry)
	{
		buki::BrowserEntry item;
		fs::path fullPath = fs::absolute(dirEntry.path()).lexically_normal();
		item.path = buki::ToAssetRelativePath(fullPath);
		item.displayName = fullPath.filename().string();
		item.isDirectory = dirEntry.is_directory();
		item.isSceneFile = !item.isDirectory && buki::IsSceneFile(item.path);
		item.isPrefabFile = !item.isDirectory && buki::IsPrefabFile(item.path);
		item.isImageFile = !item.isDirectory && buki::IsImageFile(item.path);
		item.isAudioFile = !item.isDirectory && buki::IsAudioFile(item.path);

		if (item.isPrefabFile) item.payloadType = "PREFAB";
		else if (item.isImageFile) item.payloadType = "IMAGE";
		else if (item.isAudioFile) item.payloadType = "AUDIO";
		else if (item.isSceneFile) item.payloadType = "SCENE";

		return item;
	}

	void SortBrowserEntries(std::vector<buki::BrowserEntry>& entries)
	{
		std::sort(entries.begin(), entries.end(),
			[](const buki::BrowserEntry& a, const buki::BrowserEntry& b)
			{
				if (a.isDirectory != b.isDirectory)
					return a.isDirectory > b.isDirectory;

				return buki::ToLowerCopy(a.displayName) < buki::ToLowerCopy(b.displayName);
			});
	}
}

bool buki::IsPrefabFile(const std::string& path)
{
	fs::path aspath = buki::ToAbsolutePath(path);
	return ToLowerCopy(aspath.extension().string()) == ".prefab";
}

bool buki::IsImageFile(const std::string& path)
{
	fs::path aspath = buki::ToAbsolutePath(path);
	const std::string ext = ToLowerCopy(aspath.extension().string());
	return ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" ||
		ext == ".tga" || ext == ".gif" || ext == ".webp";
}

bool buki::IsAudioFile(const std::string& path)
{
	fs::path aspath = buki::ToAbsolutePath(path);
	const std::string ext = ToLowerCopy(aspath.extension().string());
	return ext == ".wav" || ext == ".mp3" || ext == ".ogg" || ext == ".flac";
}

bool buki::IsSceneFile(const std::string& path)
{
	fs::path aspath = buki::ToAbsolutePath(path);
	const std::string ext = aspath.extension().string();
	return ext == ".json" || ext == ".scene";
}

std::string buki::ToAssetRelativePath(const fs::path& fullPath)
{
	const std::string normalized = fullPath.lexically_normal().generic_string();
	const std::string lower = ToLowerCopy(normalized);

	const std::string assetsMarker = "assets/";
	const size_t assetsPos = lower.find(assetsMarker);
	if (assetsPos != std::string::npos)
		return normalized.substr(assetsPos);

	const std::string deploymentMarker = "deployment/";
	const size_t deploymentPos = lower.find(deploymentMarker);
	if (deploymentPos != std::string::npos)
		return normalized.substr(deploymentPos + deploymentMarker.size());

	return normalized;
}

fs::path buki::ToAbsolutePath(const std::string& assetRelativePath)
{
	return fs::absolute(fs::path("../Deployment") / assetRelativePath).lexically_normal();
}

std::string buki::DisplayNameForPath(const std::string& path)
{
	if (path.empty())
		return "None";

	return fs::path(path).filename().string();
}

std::string buki::FolderSuffixForPath(const std::string& path)
{
	const fs::path parent = fs::path(path).parent_path();
	if (parent.empty())
		return "";

	return parent.generic_string();
}

std::string buki::GetFileName(const std::string& path)
{
	return fs::absolute("../Deployment/" + path).filename().string();
}

std::string buki::ToLowerCopy(std::string value)
{
	std::transform(value.begin(), value.end(), value.begin(),
		[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return value;
}

buki::BrowserVisualType buki::GetBrowserVisualType(const std::string& path, bool isDirectory)
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

		icons.folder = textures.Load("EditorImg/Folder.png");
		icons.scene = textures.Load("EditorImg/Network.png");
		icons.audio = textures.Load("EditorImg/Audio.png");
		icons.file = textures.Load("EditorImg/File.png");
		icons.prefab = textures.Load("EditorImg/Portrait.png");
		icons.image = textures.Load("EditorImg/Landscape.png");

		loaded = true;
	}

	return icons;
}

const buki::Texture2D* buki::TryGetPreviewTexture(const std::string& path)
{
	if (!IsImageFile(path))
		return nullptr;

	auto& textures = Engine::Get().Textures();
	Texture2D* texture = textures.Get(path);

	if (texture == nullptr)
		texture = textures.Load(path);

	if (texture == nullptr)
		texture = textures.Load(path, true);

	if (texture == nullptr || !texture->IsValid())
		return nullptr;

	return texture;
}

const buki::Texture2D* buki::GetBrowserThumbnail(const std::string& path, bool isDirectory)
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
	case BrowserVisualType::Scene:  return icons.scene;
	case BrowserVisualType::Prefab: return icons.prefab;
	case BrowserVisualType::Audio:  return icons.audio;
	case BrowserVisualType::Image:  return icons.file;
	case BrowserVisualType::File:   return icons.file;
	default:                        return icons.file;
	}
}

std::vector<buki::BrowserEntry> buki::CollectBrowserEntries(const fs::path& root, bool recursive)
{
	std::vector<BrowserEntry> results;

	if (!fs::exists(root) || !fs::is_directory(root))
		return results;

	if (recursive)
	{
		for (const auto& entry : fs::recursive_directory_iterator(root))
		{
			results.push_back(MakeBrowserEntry(entry));
		}
	}
	else
	{
		for (const auto& entry : fs::directory_iterator(root))
		{
			results.push_back(MakeBrowserEntry(entry));
		}
	}

	SortBrowserEntries(results);
	return results;
}

std::vector<buki::BrowserEntry> buki::CollectBrowserEntries(const fs::path& root, const char* payloadType)
{
	std::vector<BrowserEntry> results;

	if (!fs::exists(root) || !fs::is_directory(root))
		return results;

	for (const auto& entry : fs::recursive_directory_iterator(root))
	{
		if (!entry.is_regular_file())
			continue;

		BrowserEntry item = MakeBrowserEntry(entry);
		if (!MatchesPayloadType(item, payloadType))
			continue;

		item.payloadType = payloadType != nullptr ? payloadType : "";
		results.push_back(std::move(item));
	}

	SortBrowserEntries(results);
	return results;
}

std::vector<std::string> buki::CollectAssetPaths(const fs::path& root, const char* payloadType)
{
	std::vector<std::string> results;
	const std::vector<BrowserEntry> entries = CollectBrowserEntries(root, payloadType);

	results.reserve(entries.size());
	for (const BrowserEntry& entry : entries)
	{
		results.push_back(ToAssetRelativePath(entry.path));
	}

	return results;
}

bool buki::ClearSceneSelection(EditorState& state)
{
	state.selectedEntity = nullptr;
	state.activeEntity = nullptr;
	state.selectedEntities.clear();
	return true;
}

bool buki::ClearPrefabSelection(EditorState& state)
{
	state.selectedPrefabEntity = nullptr;
	state.selectedPrefabPath.clear();
	return true;
}

ImTextureID buki::ToImGuiTextureID(std::uint32_t textureId)
{
	return static_cast<ImTextureID>(textureId);
}

bool buki::DrawBrowserTile(const BrowserEntry& entry, float thumbnailSize, bool selected)
{
	bool activated = false;

	ImGui::PushID(entry.path.c_str());
	ImGui::BeginGroup();

	const Texture2D* thumbnail = GetBrowserThumbnail(entry.path, entry.isDirectory);

	ImVec2 buttonSize(thumbnailSize, thumbnailSize);
	ImVec2 imageSize(thumbnailSize, thumbnailSize);
	ImVec2 padding(thumbnailSize / 8.0f, thumbnailSize / 8.0f);

	imageSize.x -= padding.x*2;
	imageSize.y -= padding.y*2;

	if (thumbnail != nullptr && thumbnail->height != 0 && thumbnail->width != 0 &&
		thumbnail->height != thumbnail->width)
	{
		const float aspect = static_cast<float>(thumbnail->width) / static_cast<float>(thumbnail->height);
		if (aspect > 1.0f)
		{
			imageSize.y /= aspect;
			padding.y = (padding.y + thumbnailSize - imageSize.y) * 0.5f;
		}
		else
		{
			imageSize.x *= aspect;
			padding.x = (padding.x + thumbnailSize - imageSize.x) * 0.5f;
		}
	}

	const ImVec4 transparent(0.0f, 0.0f, 0.0f, 0.0f);
	const ImVec4 selectedTint(1.0f, 1.0f, 1.0f, 1.0f);
	const ImVec4 normalTint(1.0f, 1.0f, 1.0f, 0.96f);

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
			buki::ToImGuiTextureID(thumbnail->id),
			imageSize,
			ImVec2(0.0f, 0.0f),
			ImVec2(1.0f, 1.0f),
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
			const std::string pathStr = buki::ToAbsolutePath(entry.path).string();
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