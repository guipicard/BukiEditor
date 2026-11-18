#include "PrefabEditorWindow.h"

#include <algorithm>

#include "ImGui.h"
#include "WorldService.h"
#include "EditorAssetEntries.h"
#include "Camera2D.h"
#include "Entity.h"

#include "EditorState.h"

#include "EditorViewportFramebuffer.h"
#include "EditorViewportHelpers.h"
#include "glad/glad.h"

namespace buki
{
	PrefabEditorWindow::PrefabEditorWindow(
		EditorState& editorState,
		const std::string& assetPath)
		: AssetEditorWindow(editorState)
	{
		m_Session.path = assetPath;

		Open(assetPath);
	}

	bool PrefabEditorWindow::LoadAsset(const std::string& path)
	{
		m_Session.path = path;

		m_Session.prefabEntity =
			Engine::Get().World().GetOrLoadPrefabEntity(path);

		return m_Session.prefabEntity != nullptr;
	}

	void PrefabEditorWindow::OnAssetLoaded()
	{
		m_Session.windowId =
			"Prefab_" + m_AssetPath;

		m_RequestDockNextToScene = true;
		m_RequestFocus = true;
	}

	void PrefabEditorWindow::Draw()
	{
		if (!m_Open)
		{
			return;
		}

		const std::string title =
			"Prefab: " +
			GetAssetFileName() +
			"###" +
			m_AssetPath;

		if (m_RequestFocus)
		{
			ImGui::SetNextWindowFocus();
		}

		bool open = m_Open;

		if (!ImGui::Begin(title.c_str(), &open))
		{
			m_Open = open;

			if (!open)
			{
				Close();
			}

			ImGui::End();
			return;
		}

		const bool focusedNow =
			m_RequestFocus ||
			ImGui::IsWindowFocused(
				ImGuiFocusedFlags_RootAndChildWindows) ||
			ImGui::IsWindowAppearing();

		m_Session.hovered = ImGui::IsWindowHovered(
			ImGuiHoveredFlags_RootAndChildWindows);

		if (focusedNow)
		{
			UpdateSelection();

			m_Session.cameraSettings.Deserialize(
				ToAssetRelativePath(m_AssetPath));
		}

		m_RequestFocus = false;
		m_RequestDockNextToScene = false;

		const ImVec2 available = ImGui::GetContentRegionAvail();

		m_Session.cameraSettings.viewportWidth =
			std::max(1.0f, available.x);

		m_Session.cameraSettings.viewportHeight =
			std::max(1.0f, available.y);

		RenderToFramebuffer();
		DrawFramebufferTexture();

		if (focusedNow)
		{
			UpdateCamera();
			SaveCamera();
		}

		ImGui::End();

		m_Open = open;

		if (!open)
		{
			Close();
		}
	}

	void PrefabEditorWindow::Reload()
	{
		const std::string previousPath = m_AssetPath;

		DestroyFramebuffer();

		m_Session.prefabEntity = nullptr;

		Open(previousPath);
	}

	void PrefabEditorWindow::OnClosed()
	{
		m_Session.path.clear();
		m_Session.windowId.clear();
		m_Session.hovered = false;
		m_Session.focused = false;
		m_Session.prefabEntity = nullptr;
		m_Session.open = false;

		DestroyFramebuffer();
	}

	void PrefabEditorWindow::UpdateSelection()
	{
		m_EditorState.selectedPrefabPath =
			ToAssetRelativePath(m_AssetPath);

		m_EditorState.selectedPrefabEntity =
			m_Session.prefabEntity;

		m_EditorState.selectedEntity = nullptr;
		m_EditorState.activeEntity = nullptr;
		m_EditorState.selectedEntities.clear();
	}

	void PrefabEditorWindow::UpdateCamera()
	{
		Camera2D* camera = Engine::Get().GetActiveCameraPtr();

		if (!camera)
		{
			return;
		}

		m_Session.cameraSettings.position =
		{
			camera->position.x,
			camera->position.y
		};

		m_Session.cameraSettings.zoom = camera->zoom;
	}

	void PrefabEditorWindow::SaveCamera()
	{
		m_Session.cameraSettings.Serialize(
			ToAssetRelativePath(m_AssetPath));
	}

	void PrefabEditorWindow::RenderToFramebuffer()
	{
		m_Session.prefabEntity =
			Engine::Get().World().GetOrLoadPrefabEntity(
				m_AssetPath);

		const int targetWidth =
			std::max(
				1,
				static_cast<int>(
					m_Session.cameraSettings.viewportWidth));

		const int targetHeight =
			std::max(
				1,
				static_cast<int>(
					m_Session.cameraSettings.viewportHeight));

		EnsureFramebuffer(targetWidth, targetHeight);

		if (m_Session.framebuffer == 0)
		{
			return;
		}

		GLint previousFramebuffer = 0;
		GLint previousViewport[4] = {};

		GLboolean depthTestWasEnabled =
			glIsEnabled(GL_DEPTH_TEST);

		GLboolean blendWasEnabled =
			glIsEnabled(GL_BLEND);

		GLboolean previousDepthMask = GL_TRUE;

		glGetIntegerv(
			GL_FRAMEBUFFER_BINDING,
			&previousFramebuffer);

		glGetIntegerv(GL_VIEWPORT, previousViewport);
		glGetBooleanv(GL_DEPTH_WRITEMASK, &previousDepthMask);

		glBindFramebuffer(
			GL_FRAMEBUFFER,
			m_Session.framebuffer);

		glViewport(
			0,
			0,
			m_Session.framebufferWidth,
			m_Session.framebufferHeight);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		glClearColor(0.08f, 0.08f, 0.10f, 1.0f);

		glClear(
			GL_COLOR_BUFFER_BIT |
			GL_DEPTH_BUFFER_BIT |
			GL_STENCIL_BUFFER_BIT);

		if (m_Session.prefabEntity != nullptr)
		{
			m_Session.prefabEntity->Draw(0.0f);
		}

		if (depthTestWasEnabled)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		if (blendWasEnabled)
		{
			glEnable(GL_BLEND);
		}
		else
		{
			glDisable(GL_BLEND);
		}

		glDepthMask(previousDepthMask);

		glBindFramebuffer(
			GL_FRAMEBUFFER,
			static_cast<GLuint>(previousFramebuffer));

		glViewport(
			previousViewport[0],
			previousViewport[1],
			previousViewport[2],
			previousViewport[3]);
	}

	void PrefabEditorWindow::EnsureFramebuffer(
		int width,
		int height)
	{
		width = std::max(1, width);
		height = std::max(1, height);

		if (m_Session.framebuffer != 0 &&
			m_Session.framebufferWidth == width &&
			m_Session.framebufferHeight == height)
		{
			return;
		}

		DestroyFramebuffer();

		glGenFramebuffers(
			1,
			&m_Session.framebuffer);

		glBindFramebuffer(
			GL_FRAMEBUFFER,
			m_Session.framebuffer);

		glGenTextures(
			1,
			&m_Session.colorTexture);

		glBindTexture(
			GL_TEXTURE_2D,
			m_Session.colorTexture);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			width,
			height,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			nullptr);

		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_MIN_FILTER,
			GL_LINEAR);

		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_MAG_FILTER,
			GL_LINEAR);

		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_S,
			GL_CLAMP_TO_EDGE);

		glTexParameteri(
			GL_TEXTURE_2D,
			GL_TEXTURE_WRAP_T,
			GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,
			m_Session.colorTexture,
			0);

		glGenRenderbuffers(
			1,
			&m_Session.depthRenderbuffer);

		glBindRenderbuffer(
			GL_RENDERBUFFER,
			m_Session.depthRenderbuffer);

		glRenderbufferStorage(
			GL_RENDERBUFFER,
			GL_DEPTH24_STENCIL8,
			width,
			height);

		glFramebufferRenderbuffer(
			GL_FRAMEBUFFER,
			GL_DEPTH_STENCIL_ATTACHMENT,
			GL_RENDERBUFFER,
			m_Session.depthRenderbuffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
			GL_FRAMEBUFFER_COMPLETE)
		{
			DestroyFramebuffer();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return;
		}

		m_Session.framebufferWidth = width;
		m_Session.framebufferHeight = height;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void PrefabEditorWindow::DestroyFramebuffer()
	{
		if (m_Session.depthRenderbuffer != 0)
		{
			glDeleteRenderbuffers(
				1,
				&m_Session.depthRenderbuffer);

			m_Session.depthRenderbuffer = 0;
		}

		if (m_Session.colorTexture != 0)
		{
			glDeleteTextures(
				1,
				&m_Session.colorTexture);

			m_Session.colorTexture = 0;
		}

		if (m_Session.framebuffer != 0)
		{
			glDeleteFramebuffers(
				1,
				&m_Session.framebuffer);

			m_Session.framebuffer = 0;
		}

		m_Session.framebufferWidth = 0;
		m_Session.framebufferHeight = 0;
	}

	void PrefabEditorWindow::DrawFramebufferTexture()
	{
		DrawViewportTexture(
			m_Session.colorTexture,
			m_Session.framebufferWidth,
			m_Session.framebufferHeight,
			m_Session.cameraSettings.displayMode,
			m_Session.cameraSettings.viewportWidth,
			m_Session.cameraSettings.viewportHeight);
	}
}