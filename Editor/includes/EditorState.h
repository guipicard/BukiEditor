#pragma once

#include <filesystem>

namespace buki
{
	class Entity;

	enum class ViewportDisplayMode
	{
		Stretch = 0,
		Fit = 1,
		OneToOne = 2
	};

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

		unsigned int sceneFramebuffer = 0;
		unsigned int sceneColorTexture = 0;
		unsigned int sceneDepthRenderbuffer = 0;
		int sceneFramebufferWidth = 0;
		int sceneFramebufferHeight = 0;

		ViewportDisplayMode viewportDisplayMode = ViewportDisplayMode::OneToOne;
		bool lockViewportRenderSize = true;
		bool canSeeViewportRenderSize = true;
		bool canSeeViewportRenderInfo = false;
		int lockedViewportWidth = 1920;
		int lockedViewportHeight = 1080;

		std::filesystem::path currentContentPath = "../Deployment";
		std::filesystem::path scenesRootPath = "../Deployment/Scenes";
		std::filesystem::path selectedScenePath;

		bool setTilesDefaults = true;
	};
}