#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include "EditorState.h"
#include "imgui.h"

namespace fs = std::filesystem;
namespace buki
{
	struct Texture2D;

	struct BrowserEntry
	{
		std::string path;
		std::string displayName;
		std::string payloadType;
		bool isDirectory = false;
		bool isSceneFile = false;
		bool isPrefabFile = false;
		bool isImageFile = false;
		bool isAudioFile = false;
	};

	enum class BrowserVisualType
	{
		Folder,
		Scene,
		Prefab,
		Audio,
		Image,
		File
	};

	struct EditorBrowserIcons
	{
		const Texture2D* folder = nullptr;
		const Texture2D* scene = nullptr;
		const Texture2D* audio = nullptr;
		const Texture2D* file = nullptr;
		const Texture2D* prefab = nullptr;
		const Texture2D* image = nullptr;
	};

	bool IsPrefabFile(const std::string& path);
	bool IsImageFile(const std::string& path);
	bool IsAudioFile(const std::string& path);
	bool IsSceneFile(const std::string& path);

	std::string ToAssetRelativePath(const fs::path& fullPath);
	fs::path ToAbsolutePath(const std::string& assetRelativePath);
	std::string DisplayNameForPath(const std::string& path);
	std::string FolderSuffixForPath(const std::string& path);
	std::string GetFileName(const std::string& path);
	std::string ToLowerCopy(std::string value);

	BrowserVisualType GetBrowserVisualType(const std::string& path, bool isDirectory);

	EditorBrowserIcons& GetEditorBrowserIcons();
	const Texture2D* TryGetPreviewTexture(const std::string& path);
	const Texture2D* GetBrowserThumbnail(const std::string& path, bool isDirectory);

	std::vector<BrowserEntry> CollectBrowserEntries(const fs::path& root, bool recursive);
	std::vector<BrowserEntry> CollectBrowserEntries(const fs::path& root, const char* payloadType);
	std::vector<std::string> CollectAssetPaths(const fs::path& root, const char* payloadType);

	bool ClearSceneSelection(EditorState& state);
	bool ClearPrefabSelection(EditorState& state);
	ImTextureID ToImGuiTextureID(std::uint32_t textureId);

	bool DrawBrowserTile(const BrowserEntry& entry, float thumbnailSize, bool selected);
}