#pragma once

#include "EditorState.h"
#include "Engine.h"
#include "AnimationAssetEditor.h"

#include <algorithm>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

namespace buki
{
	struct Texture2D;
	struct BrowserEntry;
	class Entity;

	class ContentBrowserPanel
	{
	public:
		void Render(EditorState& state);
	};
}