#pragma once

#include <filesystem>

namespace buki
{
	struct EditorState
	{
		int selectedEntityId = -1;

		bool showHierarchy = true;
		bool showInspector = true;
		bool showSceneView = true;
		bool showContentBrowser = true;
		bool showDemoWindow = false;

		float viewportWidth = 0.0f;
		float viewportHeight = 0.0f;
		bool viewportFocused = false;
		bool viewportHovered = false;

		std::filesystem::path currentContentPath = "Assets";
	};
}