#pragma once

#include "EditorState.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "SceneViewPanel.h"
#include "ContentBrowserPanel.h"
#include "imgui.h"

namespace buki
{
	class EditorLayer
	{
	public:
		EditorLayer() = default;
		void Render();

	private:
		void DrawDockspace();
		void BuildDefaultLayout(ImGuiID dockspaceId);

	private:
		EditorState state;
		HierarchyPanel hierarchyPanel;
		InspectorPanel inspectorPanel;
		SceneViewPanel sceneViewPanel;
		ContentBrowserPanel contentBrowserPanel;

		bool firstLayout = true;
	};
}