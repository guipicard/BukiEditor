#pragma once

#include "EditorState.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "SceneViewPanel.h"
#include "ContentBrowserPanel.h"
#include "ImGui.h"
#include "Camera2D.h"
#include "IWorld.h"
#include <vector>

namespace buki
{
	class EditorLayer
	{
	public:
		EditorLayer();
		~EditorLayer();
		void Render();
		void RenderSceneViewport();
		EditorState& State() { return state; }

	private:
		void DrawDockspace();
		void BuildDefaultLayout(ImGuiID dockspaceId);
		void DrawMenuBar();

	private:
		EditorState state;
		HierarchyPanel hierarchyPanel;
		InspectorPanel inspectorPanel;
		SceneViewPanel sceneViewPanel;
		ContentBrowserPanel contentBrowserPanel;

		bool firstLayout = true;
	};
}
