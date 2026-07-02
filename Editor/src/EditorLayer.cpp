#include "EditorLayer.h"

#include "Engine.h"
#include "Entity.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "EditorAssetEntries.h"

#include "EditorViewportFramebuffer.h"
#include "EditorViewportHelpers.h"
#include <glad/glad.h>

buki::EditorLayer::~EditorLayer()
{
	for (auto& session : state.prefabPreviewSessions)
		DestroyPrefabPreviewFramebuffer(session);
}

void buki::EditorLayer::Render()
{
	DrawDockspace();

	if (state.showHierarchy)
		hierarchyPanel.Render(state);

	if (state.showInspector)
		inspectorPanel.Render(state);

	if (state.showSceneView)
		sceneViewPanel.Render(state);

	RenderPrefabPanels();
	RenderSceneViewport();
	RenderPrefabPreviewViewports();

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

	state.prefabDockNodeId = dock_main_id;

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

void buki::EditorLayer::RenderPrefabPreviewViewports()
{
	for (auto& session : state.prefabPreviewSessions)
	{
		if (!session.open)
			continue;

		if (session.cameraSettings.viewportWidth <= 0.0f || session.cameraSettings.viewportHeight <= 0.0f)
			continue;

		RenderPrefabSessionToFramebuffer(session);
	}
}

void buki::EditorLayer::RenderPrefabSessionToFramebuffer(buki::PrefabPreviewSession& session)
{
	session.prefabEntity = Engine::Get().World().GetOrLoadPrefabEntity(session.path);
	const int targetWidth = std::max(1, static_cast<int>(session.cameraSettings.viewportWidth));
	const int targetHeight = std::max(1, static_cast<int>(session.cameraSettings.viewportHeight));

	EnsurePrefabPreviewFramebuffer(session, targetWidth, targetHeight);
	if (session.framebuffer == 0)
		return;

	GLint previousFramebuffer = 0;
	GLint previousViewport[4] = {};
	GLboolean depthTestWasEnabled = glIsEnabled(GL_DEPTH_TEST);
	GLboolean blendWasEnabled = glIsEnabled(GL_BLEND);
	GLboolean previousDepthMask = GL_TRUE;

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);
	glGetIntegerv(GL_VIEWPORT, previousViewport);
	glGetBooleanv(GL_DEPTH_WRITEMASK, &previousDepthMask);

	glBindFramebuffer(GL_FRAMEBUFFER, session.framebuffer);
	glViewport(0, 0, session.framebufferWidth, session.framebufferHeight);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if (session.prefabEntity != nullptr)
	{
		session.prefabEntity->Draw(0.0f);

	}

	if (depthTestWasEnabled) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (blendWasEnabled) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	glDepthMask(previousDepthMask);

	glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(previousFramebuffer));
	glViewport(previousViewport[0], previousViewport[1], previousViewport[2], previousViewport[3]);
}

void buki::EditorLayer::EnsurePrefabPreviewFramebuffer(PrefabPreviewSession& session, int width, int height)
{
	width = std::max(1, width);
	height = std::max(1, height);

	if (session.framebuffer != 0 &&
		session.framebufferWidth == width &&
		session.framebufferHeight == height)
	{
		return;
	}

	DestroyPrefabPreviewFramebuffer(session);

	glGenFramebuffers(1, &session.framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, session.framebuffer);

	glGenTextures(1, &session.colorTexture);
	glBindTexture(GL_TEXTURE_2D, session.colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, session.colorTexture, 0);

	glGenRenderbuffers(1, &session.depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, session.depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, session.depthRenderbuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		DestroyPrefabPreviewFramebuffer(session);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return;
	}

	session.framebufferWidth = width;
	session.framebufferHeight = height;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void buki::EditorLayer::DestroyPrefabPreviewFramebuffer(PrefabPreviewSession& session)
{
	if (session.depthRenderbuffer != 0)
	{
		glDeleteRenderbuffers(1, &session.depthRenderbuffer);
		session.depthRenderbuffer = 0;
	}

	if (session.colorTexture != 0)
	{
		glDeleteTextures(1, &session.colorTexture);
		session.colorTexture = 0;
	}

	if (session.framebuffer != 0)
	{
		glDeleteFramebuffers(1, &session.framebuffer);
		session.framebuffer = 0;
	}

	session.framebufferWidth = 0;
	session.framebufferHeight = 0;
}

void buki::EditorLayer::RenderPrefabPanels()
{
	for (size_t i = 0; i < state.prefabPreviewSessions.size();)
	{
		PrefabPreviewSession& session = state.prefabPreviewSessions[i];

		if (!session.open)
		{
			DestroyPrefabPreviewFramebuffer(session);
			state.prefabPreviewSessions.erase(state.prefabPreviewSessions.begin() + i);

			if (state.activePrefabPreviewIndex == static_cast<int>(i))
				state.activePrefabPreviewIndex = -1;
			else if (state.activePrefabPreviewIndex > static_cast<int>(i))
				--state.activePrefabPreviewIndex;

			continue;
		}

		std::string title =
			"Prefab: " + fs::path(session.path).filename().string() + "###" + session.windowId;

		bool open = session.open;

		if (session.requestDockNextToScene && state.prefabDockNodeId != 0)
		{
			ImGui::SetNextWindowDockID(state.prefabDockNodeId, ImGuiCond_Always);
		}

		if (session.requestFocus)
		{
			ImGui::SetNextWindowFocus();
		}

		if (ImGui::Begin(title.c_str(), &open))
		{
			if (session.requestFocus)
			{
				ImGui::FocusWindow(ImGui::GetCurrentWindow());
			}

			const bool focusedNow =
				session.requestFocus ||
				ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) ||
				ImGui::IsWindowAppearing();

			session.hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

			if (focusedNow)
			{
				for (size_t j = 0; j < state.prefabPreviewSessions.size(); ++j)
				{
					state.prefabPreviewSessions[j].focused = (j == i);
				}

				state.activePrefabPreviewIndex = static_cast<int>(i);
				state.selectedPrefabPath = buki::ToAssetRelativePath(session.path);
				state.selectedPrefabEntity = session.prefabEntity;
				state.selectedEntity = nullptr;
				state.activeEntity = nullptr;
				state.selectedEntities.clear();

				session.cameraSettings.Deserialize(session.path);
			}
			else
			{
				session.focused = false;
			}

			session.requestFocus = false;
			session.requestDockNextToScene = false;

			ImVec2 avail = ImGui::GetContentRegionAvail();
			session.cameraSettings.viewportWidth = std::max(1.0f, avail.x);
			session.cameraSettings.viewportHeight = std::max(1.0f, avail.y);

			DrawPrefabSessionTexture(session);

			if (focusedNow)
			{
				auto pos = Engine::Get().GetActiveCameraPtr()->position;
				session.cameraSettings.position = { pos.x, pos.y };
				session.cameraSettings.zoom = Engine::Get().GetActiveCameraPtr()->zoom;
				session.cameraSettings.Serialize(buki::ToAssetRelativePath(session.path));
			}
		}
		ImGui::End();

		session.open = open;

		++i;
	}
}