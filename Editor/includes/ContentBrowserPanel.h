#pragma once

#include "Engine.h"
#include "EditorCanvasNavigation.h"

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
	struct EditorState;
	class Entity;

	class ContentBrowserPanel
	{
	public:
		void Render(EditorState& state);

	};
}
