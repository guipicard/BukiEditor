#include "EditorLayer.h"

#include "imgui_internal.h"

void buki::EditorLayer::Render()
{
	DrawDockspace();

	if (state.showHierarchy)
		hierarchyPanel.Render(state);

	if (state.showInspector)
		inspectorPanel.Render(state);

	if (state.showSceneView)
		sceneViewPanel.Render(state);

	if (state.showContentBrowser)
		contentBrowserPanel.Render(state);

	if (state.showDemoWindow)
		ImGui::ShowDemoWindow(&state.showDemoWindow);
}

void buki::EditorLayer::DrawDockspace()
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	window_flags |= ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("MainDockSpace", nullptr, window_flags);

	ImGui::PopStyleVar(3);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Save Scene", nullptr, false, false);
			ImGui::MenuItem("Open Scene", nullptr, false, false);
			ImGui::Separator();
			ImGui::MenuItem("Exit", nullptr, false, false);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem("Hierarchy", nullptr, &state.showHierarchy);
			ImGui::MenuItem("Inspector", nullptr, &state.showInspector);
			ImGui::MenuItem("Scene View", nullptr, &state.showSceneView);
			ImGui::MenuItem("Content Browser", nullptr, &state.showContentBrowser);
			ImGui::Separator();
			ImGui::MenuItem("ImGui Demo", nullptr, &state.showDemoWindow);
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGuiID dockspaceId = ImGui::GetID("BukiEditorDockspace");
	ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	if (firstLayout)
	{
		firstLayout = false;
		BuildDefaultLayout(dockspaceId);
	}

	ImGui::End();
}

void buki::EditorLayer::BuildDefaultLayout(ImGuiID dockspaceId)
{
	ImGui::DockBuilderRemoveNode(dockspaceId);
	ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
	ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->WorkSize);

	ImGuiID dockMain = dockspaceId;
	ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.20f, nullptr, &dockMain);
	ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, 0.25f, nullptr, &dockMain);
	ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.25f, nullptr, &dockMain);

	ImGui::DockBuilderDockWindow("Hierarchy", dockLeft);
	ImGui::DockBuilderDockWindow("Inspector", dockRight);
	ImGui::DockBuilderDockWindow("Scene View", dockMain);
	ImGui::DockBuilderDockWindow("Content Browser", dockBottom);

	ImGui::DockBuilderFinish(dockspaceId);
}