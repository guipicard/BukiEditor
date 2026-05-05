#include "EditorLayer.h"

#include "Engine.h"
#include "Entity.h"
#include "imgui.h"
#include "imgui_internal.h"

#include "EditorViewportFramebuffer.h"
#include <glad/glad.h>

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

	RenderSceneViewport();

	if (state.showDemoWindow)
	{
		ImGui::ShowDemoWindow(&state.showDemoWindow);
	}
}

void buki::EditorLayer::RenderSceneViewport()
{
	int renderWidth = static_cast<int>(state.viewportWidth);
	int renderHeight = static_cast<int>(state.viewportHeight);

	if (state.lockViewportRenderSize)
	{
		renderWidth = state.lockedViewportWidth;
		renderHeight = state.lockedViewportHeight;
	}

	if (renderWidth <= 0 || renderHeight <= 0)
	{
		return;
	}


	if (!EditorViewportFramebuffer::Resize(state, renderWidth, renderHeight))
	{
		return;
	}


	GLint previousFramebuffer = 0;
	GLint previousViewport[4] = {};
	GLboolean depthTestWasEnabled = glIsEnabled(GL_DEPTH_TEST);
	GLboolean blendWasEnabled = glIsEnabled(GL_BLEND);
	GLboolean previousDepthMask = GL_TRUE;

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);
	glGetIntegerv(GL_VIEWPORT, previousViewport);
	glGetBooleanv(GL_DEPTH_WRITEMASK, &previousDepthMask);

	EditorViewportFramebuffer::Bind(state);

	glViewport(0, 0, state.sceneFramebufferWidth, state.sceneFramebufferHeight);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Engine::Get().World().Render(1.0f);

	if (depthTestWasEnabled)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	glDepthMask(previousDepthMask);

	if (blendWasEnabled)
	{
		glEnable(GL_BLEND);
	}
	else
	{
		glDisable(GL_BLEND);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(previousFramebuffer));
	glViewport(previousViewport[0], previousViewport[1], previousViewport[2], previousViewport[3]);
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
	ImGuiID dock_demo_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.35f, nullptr, &dock_main_id);

	ImGui::DockBuilderDockWindow("Hierarchy", dock_left_id);
	ImGui::DockBuilderDockWindow("Inspector", dock_right_id);
	ImGui::DockBuilderDockWindow("Scene", dock_main_id);
	ImGui::DockBuilderDockWindow("ImGui Demo", dock_demo_id);
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
