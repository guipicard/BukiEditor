#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace buki
{
	struct Texture2D;

	struct BrowserEntry
	{
		fs::path fullPath;
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
	};

	std::string ToLowerCopy(std::string value);

	bool IsPrefabFile(const fs::path& path);
	bool IsImageFile(const fs::path& path);
	bool IsAudioFile(const fs::path& path);
	bool IsSceneFile(const fs::path& path);

	std::string ToAssetRelativePath(const fs::path& fullPath);
	std::string DisplayNameForPath(const std::string& path);
	std::string FolderSuffixForPath(const std::string& path);

	BrowserVisualType GetBrowserVisualType(const fs::path& path, bool isDirectory);

	EditorBrowserIcons& GetEditorBrowserIcons();
	const Texture2D* TryGetPreviewTexture(const fs::path& path);
	const Texture2D* GetBrowserThumbnail(const fs::path& path, bool isDirectory);

	std::vector<BrowserEntry> CollectBrowserEntries(const fs::path& root, bool recursive);
	std::vector<BrowserEntry> CollectBrowserEntries(const fs::path& root, const char* payloadType);
	std::vector<std::string> CollectAssetPaths(const fs::path& root, const char* payloadType);

	bool DrawBrowserTile(const BrowserEntry& entry, float thumbnailSize, bool selected);
}