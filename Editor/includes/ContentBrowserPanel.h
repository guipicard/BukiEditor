#pragma once

#include "EditorState.h"
#include "Engine.h"

#include <algorithm>
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
		const Texture2D* network = nullptr;
		const Texture2D* audio = nullptr;
		const Texture2D* file = nullptr;
	};

	BrowserVisualType GetBrowserVisualType(const fs::path& path, bool isDirectory);
	EditorBrowserIcons& GetEditorBrowserIcons();
	const Texture2D* GetBrowserThumbnail(const fs::path& path, bool isDirectory);

	std::string ToLowerCopy(std::string value);
	bool IsPrefabFile(const fs::path& path);
	bool IsImageFile(const fs::path& path);
	bool IsAudioFile(const fs::path& path);
	bool IsSceneFile(const fs::path& path);

	const Texture2D* TryGetPreviewTexture(const fs::path& path);
	bool DrawBrowserTile(const BrowserEntry& entry, float thumbnailSize, bool selected);

	class ContentBrowserPanel
	{
	public:
		void Render(EditorState& state);
	};
}