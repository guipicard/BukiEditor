#pragma once

#include "EditorState.h"
#include "Engine.h"

#include <algorithm>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

namespace buki
{
	struct Texture2D;
	class Entity;

	class ContentBrowserPanel
	{
	public:
		void Render(EditorState& state);
	private:
		void SelectPrefab(const std::filesystem::path& path, EditorState& state);
		bool OpenPrefabPreview(const std::filesystem::path& path, EditorState& state);
	};
}