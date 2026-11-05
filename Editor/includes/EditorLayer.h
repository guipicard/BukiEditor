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
		EditorLayer() = default;
		~EditorLayer();
		void Render();
		void RenderSceneViewport();
		EditorState& State() { return state; }

	private:
		void DrawDockspace();
		void BuildDefaultLayout(ImGuiID dockspaceId);
		void DrawMenuBar();

		void RenderPrefabPreviewViewports();
		void RenderPrefabSessionToFramebuffer(PrefabPreviewSession& session);
		void EnsurePrefabPreviewFramebuffer(PrefabPreviewSession& session, int width, int height);
		void DestroyPrefabPreviewFramebuffer(PrefabPreviewSession& session);

		void RenderPrefabPanels();
	private:
		EditorState state;
		HierarchyPanel hierarchyPanel;
		InspectorPanel inspectorPanel;
		SceneViewPanel sceneViewPanel;
		ContentBrowserPanel contentBrowserPanel;

		bool firstLayout = true;
	};
}
