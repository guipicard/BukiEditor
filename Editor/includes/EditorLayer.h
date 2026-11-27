#pragma once

#include "EditorState.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "SceneViewPanel.h"
#include "ContentBrowserPanel.h"
#include "ImGui.h"
#include "Camera2D.h"
#include "IWorld.h"
#include "EditorConsole.h"
#include "GameProcess.h"
#include <vector>

namespace buki
{
	enum class PlayState
	{
		Idle,
		Building,
		Running
	};
	class EditorLayer
	{
	public:
		EditorLayer();
		~EditorLayer();
		void Render();
		void RenderSceneViewport();
		EditorState& State() { return state; }

		void StartPlay();
		void StopPlay();

	private:
		void DrawDockspace();
		void BuildDefaultLayout(ImGuiID dockspaceId);
		void DrawMenuBar();

		void LaunchGame();
		void UpdatePlay();
		void CleanupPlayScene();
		void DrawPlayControls();

		EditorConsole console;
		GameProcess buildProcess;
		GameProcess gameProcess;
		PlayState playState = PlayState::Idle;
		bool buildBeforePlay = true;
		bool showConsole = true;
		std::string playScenePath;


		EditorState state;
		HierarchyPanel hierarchyPanel;
		InspectorPanel inspectorPanel;
		SceneViewPanel sceneViewPanel;
		ContentBrowserPanel contentBrowserPanel;

		bool firstLayout = true;
	};
}
