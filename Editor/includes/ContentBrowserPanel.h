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

	class ContentBrowserPanel
	{
	public:
		void Render(EditorState& state);
	};
}