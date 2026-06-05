#pragma once

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include "nlohmann/json.hpp"
#include "BukiContainers.h"
#include "Engine.h"
#include "ImGui.h"

namespace fs = std::filesystem;
using json = nlohmann::json;


namespace buki
{
	class Entity;

	enum class ViewportDisplayMode
	{
		Stretch = 0,
		Fit = 1,
		OneToOne = 2
	};

	class CameraSettings
	{
	public:
		CameraSettings() = default;
		~CameraSettings() = default;
		Vector2 position = { 0.0f, 0.0f };
		float zoom = 1.0f;
		float viewportWidth = 0.0f;
		float viewportHeight = 0.0f;
		ViewportDisplayMode displayMode = ViewportDisplayMode::Fit;
		void Serialize(std::filesystem::path path)
		{
			const std::string savePath = "../Deployment/Editor.json";
			std::filesystem::path asPath(std::filesystem::absolute(savePath));
			std::string fileName = path.stem().string();
			std::string extension = path.extension().string();

			json doc = json::object();
			std::ifstream in(asPath);
			if (in.is_open())
			{
				try { in >> doc; }
				catch (...) { doc = json::object(); }
			}

			const std::string key = fs::absolute(path).lexically_normal().string();
			doc[key]["position"]["x"] = position.x;
			doc[key]["position"]["y"] = position.y;
			doc[key]["zoom"] = zoom;
			doc[key]["viewportWidth"] = viewportWidth;
			doc[key]["viewportHeight"] = viewportHeight;
			doc[key]["displayMode"] = static_cast<int>(displayMode);
			doc[key]["type"] = path.extension().string();
			doc[key]["path"] = fs::absolute(path).lexically_normal().string();

			std::ofstream out(asPath, std::ios::out | std::ios::trunc);
			if (out.is_open())
				out << doc.dump(4);
		}
		void Deserialize(std::filesystem::path path)
		{
			const std::filesystem::path asPath = std::filesystem::absolute("../Deployment/Editor.json");
			json doc = json::object();

			std::ifstream in(asPath);
			if (!in.is_open())
				return;

			try { in >> doc; }
			catch (...) { return; }

			const std::string key = fs::absolute(path).lexically_normal().string();
			if (!doc.contains(key))
				return;

			const json& node = doc[key];

			position.x = node.value("position", json::object()).value("x", 0.0f);
			position.y = node.value("position", json::object()).value("y", 0.0f);
			zoom = node.value("zoom", 1.0f);
			viewportWidth = node.value("viewportWidth", 0.0f);
			viewportHeight = node.value("viewportHeight", 0.0f);
			displayMode = static_cast<ViewportDisplayMode>(node.value("displayMode", static_cast<int>(ViewportDisplayMode::Fit)));

			Engine::Get().GetActiveCameraPtr()->position = { position.x, position.y };
			Engine::Get().GetActiveCameraPtr()->zoom = zoom;
		}
	};

	struct PrefabPreviewSession
	{
		std::filesystem::path path;
		Entity* prefabEntity = nullptr;
		bool open = true;
		bool focused = false;
		bool hovered = false;
		bool requestFocus = false;
		bool requestDockNextToScene = false;
		std::string windowId;

		unsigned int framebuffer = 0;
		unsigned int colorTexture = 0;
		unsigned int depthRenderbuffer = 0;
		int framebufferWidth = 0;
		int framebufferHeight = 0;

		CameraSettings cameraSettings;
	};

	struct ScenePreviewSession
	{
		std::filesystem::path path;
		std::vector<Entity*> prefabEntity;
		bool open = true;
		bool focused = false;
		bool hovered = false;
		std::string windowId;

		unsigned int framebuffer = 0;
		unsigned int colorTexture = 0;
		unsigned int depthRenderbuffer = 0;
		int framebufferWidth = 0;
		int framebufferHeight = 0;

		CameraSettings cameraSettings;
	};

	enum class EditorContextType
	{
		Scene,
		Prefab
	};

	struct EditorContext
	{
		EditorContextType type = EditorContextType::Scene;
		std::filesystem::path path;
		Entity* prefabEntity = nullptr;
	};

	struct EditorState
	{
		Entity* selectedEntity = nullptr;
		std::vector<Entity*> selectedEntities;
		Entity* activeEntity = nullptr;

		bool showHierarchy = true;
		bool showInspector = true;
		bool showSceneView = true;
		bool showContentBrowser = true;
		bool showDemoWindow = false;

		bool requestSceneWindowFocus = false;

		bool sceneDirty = false;

		float viewportWidth = 0.0f;
		float viewportHeight = 0.0f;

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

		std::filesystem::path selectedPrefabPath;
		Entity* selectedPrefabEntity = nullptr;

		std::vector<PrefabPreviewSession> prefabPreviewSessions;
		int activePrefabPreviewIndex = -1;

		ScenePreviewSession scenePreviewSession;

		ImGuiID prefabDockNodeId = 0;
	};
}
