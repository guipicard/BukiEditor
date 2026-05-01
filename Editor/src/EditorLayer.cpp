#include "EditorLayer.h"

#include "Engine.h"
#include "imgui.h"
#include "imgui_internal.h"

void buki::EditorLayer::Render()
{
	DrawDockspace();

	if (state.showHierarchy)
	{
		hierarchyPanel.Render(state);
	}

	if (state.showInspector)
	{
		inspectorPanel.Render(state);
	}

	if (state.showSceneView)
	{
		sceneViewPanel.Render(state);
	}

	if (state.showContentBrowser)
	{
		contentBrowserPanel.Render(state);
	}

	if (state.showDemoWindow)
	{
		ImGui::ShowDemoWindow(&state.showDemoWindow);
	}
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

	ImGui::Begin("EditorDockspaceRoot", nullptr, window_flags);
	ImGui::PopStyleVar(3);

	DrawMenuBar();

	ImGuiID dockspace_id = ImGui::GetID("EditorDockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	if (firstLayout)
	{
		BuildDefaultLayout(dockspace_id);
		firstLayout = false;
	}

	ImGui::End();
}

void buki::EditorLayer::BuildDefaultLayout(ImGuiID dockspaceId)
{
	ImGui::DockBuilderRemoveNode(dockspaceId);
	ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
	ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->WorkSize);

	ImGuiID dock_main_id = dockspaceId;
	ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.22f, nullptr, &dock_main_id);
	ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.28f, nullptr, &dock_main_id);
	ImGuiID dock_bottom_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);

	ImGui::DockBuilderDockWindow("Hierarchy", dock_left_id);
	ImGui::DockBuilderDockWindow("Inspector", dock_right_id);
	ImGui::DockBuilderDockWindow("Scene", dock_main_id);
	ImGui::DockBuilderDockWindow("Content Browser", dock_bottom_id);

	ImGui::DockBuilderFinish(dockspaceId);
}

void buki::EditorLayer::DrawMenuBar()
{
	if (!ImGui::BeginMenuBar())
	{
		return;
	}

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Load Selected Scene", nullptr, false, !state.selectedScenePath.empty()))
		{
			auto* world = buki::Engine::Get().GetWorldPtr();
			if (world != nullptr && world->LoadScene(state.selectedScenePath.string()))
			{
				state.sceneDirty = false;
				state.selectedEntity = nullptr;
			}
		}

		if (ImGui::MenuItem("Save Selected Scene", nullptr, false, !state.selectedScenePath.empty()))
		{
			auto* world = buki::Engine::Get().GetWorldPtr();
			if (world != nullptr && world->SaveScene(state.selectedScenePath.string()))
			{
				state.sceneDirty = false;
			}
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Window"))
	{
		ImGui::MenuItem("Hierarchy", nullptr, &state.showHierarchy);
		ImGui::MenuItem("Inspector", nullptr, &state.showInspector);
		ImGui::MenuItem("Scene", nullptr, &state.showSceneView);
		ImGui::MenuItem("Content Browser", nullptr, &state.showContentBrowser);
		ImGui::MenuItem("ImGui Demo", nullptr, &state.showDemoWindow);
		ImGui::EndMenu();
	}

	ImGui::Separator();

	if (state.selectedScenePath.empty())
	{
		ImGui::TextUnformatted("Scene: <none>");
	}
	else
	{
		ImGui::Text("Scene: %s", state.selectedScenePath.filename().string().c_str());
	}

	ImGui::Separator();
	ImGui::TextUnformatted(state.sceneDirty ? "Modified" : "Saved");

	ImGui::EndMenuBar();
}
