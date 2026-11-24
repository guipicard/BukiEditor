#pragma once
#include "EditorLayer.h"

#include "Engine.h"
#include "Entity.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "EditorAssetEntries.h"
#include "AssetType.h"
#include "AssetBehaviors.h"
#include "AssetEditorWindowManager.h"

#include "EditorViewportFramebuffer.h"
#include "EditorViewportHelpers.h"
#include "glad/glad.h"

buki::EditorLayer::EditorLayer()
{
	state.assetBehaviors.Register(AssetType::Clip, std::make_unique<ClipAssetBehavior>());

	state.assetBehaviors.Register(AssetType::Anim, std::make_unique<AnimationAssetBehavior>());

	state.assetBehaviors.Register(AssetType::Prefab, std::make_unique<PrefabAssetBehavior>());

	state.assetBehaviors.Register(AssetType::Scene, std::make_unique<SceneAssetBehavior>());

	state.assetDatabase.SetBehaviorRegistry(&state.assetBehaviors);
	state.assetDatabase.ScanDirectory("../Deployment");
}

buki::EditorLayer::~EditorLayer()
{
}

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

	state.assetEditorWindows.DrawWindows();
	state.assetEditorWindows.RemoveClosedWindows();

	RenderSceneViewport();

	if (state.showContentBrowser)
		contentBrowserPanel.Render(state);


	if (state.showDemoWindow)
		ImGui::ShowDemoWindow(&state.showDemoWindow);
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

	Camera2D* camera = Engine::Get().GetActiveCameraPtr();

	if (camera)
	{
		const Camera2D previousCamera = *camera;

		const auto& settings = state.scenePreviewSession.cameraSettings;

		camera->position =
		{
			settings.position.x,
			settings.position.y
		};

		camera->zoom = settings.zoom;

		camera->viewportWidth = static_cast<float>(state.sceneFramebufferWidth);

		camera->viewportHeight = static_cast<float>(state.sceneFramebufferHeight);

		Engine::Get().World().Render(1.0f);

		*camera = previousCamera;
	}
	else
	{
		Engine::Get().World().Render(1.0f);
	}

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

	state.assetEditorWindows.SetAssetDockNodeId(dock_main_id);

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
				state.MarkSceneDirty();
				state.selectedEntity = nullptr;
			}
		}

		if (ImGui::MenuItem("Save Selected Scene", nullptr, false, !state.selectedScenePath.empty()))
		{
			auto* world = buki::Engine::Get().GetWorldPtr();
			if (world != nullptr && world->SaveScene(state.selectedScenePath.string()))
			{
				state.MarkSceneDirty();
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
	ImGui::TextUnformatted(state.IsSceneDirty() ? "Modified" : "Saved");

	ImGui::EndMenuBar();
}
