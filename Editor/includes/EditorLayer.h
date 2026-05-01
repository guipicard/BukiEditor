#pragma once

#include "EditorState.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "SceneViewPanel.h"
#include "ContentBrowserPanel.h"

typedef unsigned int ImGuiID;
namespace buki
{
	class EditorLayer
	{
	public:
		EditorLayer() = default;
		void Render();

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
