#pragma once

#include <filesystem>

namespace buki
{
	class Entity;

	struct EditorState
	{
		Entity* selectedEntity = nullptr;

		bool showHierarchy = true;
		bool showInspector = true;
		bool showSceneView = true;
		bool showContentBrowser = true;
		bool showDemoWindow = false;

		bool sceneDirty = false;

		float viewportWidth = 0.0f;
		float viewportHeight = 0.0f;
		bool viewportFocused = false;
		bool viewportHovered = false;

		std::filesystem::path currentContentPath = "../Deployment";
		std::filesystem::path scenesRootPath = "../Deployment/Scenes";
		std::filesystem::path selectedScenePath;
	};
}