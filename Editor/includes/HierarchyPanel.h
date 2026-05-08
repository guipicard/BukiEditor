#pragma once

#include "EditorState.h"
#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

namespace buki
{
	class HierarchyPanel
	{
	public:
		void Render(EditorState& state);
		void SaveEntityAsPrefab(Entity* entity, const std::filesystem::path& path);
	};
}