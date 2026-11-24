#pragma once
#include "SceneViewPanel.h"

#include "EditorViewportHelpers.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "Engine.h"
#include "Entity.h"
#include "IAssetBehavior.h"
#include "AssetType.h"
#include "EditorViewportNavigation.h"

#include <cstdint>
#include <algorithm>
#include <fstream>

namespace fs = std::filesystem;

namespace
{
	static void ApplyViewportPreset(buki::EditorState& state, int width, int height)
	{
		state.lockViewportRenderSize = true;
		state.lockedViewportWidth = width;
		state.lockedViewportHeight = height;
	}

	static void ClampViewportSize(buki::EditorState& state)
	{
		if (state.lockedViewportWidth < 1) state.lockedViewportWidth = 1;
		if (state.lockedViewportHeight < 1) state.lockedViewportHeight = 1;
	}
}

void buki::SceneViewPanel::Render(EditorState& state)
{
	if (state.requestSceneWindowFocus)
	{
		ImGui::SetNextWindowFocus();
	}

	if (ImGui::Begin("Scene", &state.showSceneView))
	{
		if (state.requestSceneWindowFocus)
		{
			ImGui::FocusWindow(ImGui::GetCurrentWindow());
			state.requestSceneWindowFocus = false;
		}

		ImGuiWindow* sceneWindow = ImGui::GetCurrentWindow();
		if (sceneWindow != nullptr && sceneWindow->DockNode != nullptr)
		{
			state.assetEditorWindows.SetAssetDockNodeId(sceneWindow->DockNode->ID);
		}

		const bool focusedNow = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) || ImGui::IsWindowAppearing();

		const bool wasFocused = !state.scenePreviewSession.focused && focusedNow;

		state.scenePreviewSession.focused = focusedNow;
		state.scenePreviewSession.hovered = ImGui::IsWindowHovered();

		if (state.selectedScenePath.empty())
		{
			ImGui::TextUnformatted("Selected scene: <none>");
		}
		else
		{
			ImGui::Text("Selected scene: %s", state.selectedScenePath.filename().string().c_str());
		}

		ImGui::Separator();

		const char* displayModes[] = { "Stretch", "Fit", "Native" };
		int currentMode = static_cast<int>(state.viewportDisplayMode);
		ImGui::SetNextItemWidth(120.0f);
		if (ImGui::Combo("Display", &currentMode, displayModes, IM_ARRAYSIZE(displayModes)))
		{
			state.viewportDisplayMode = static_cast<ViewportDisplayMode>(currentMode);
		}

		ImGui::SameLine();
		ImGui::Checkbox("Lock Render Size", &state.lockViewportRenderSize);
		ImGui::SameLine();
		ImGui::Checkbox("Show Render Info", &state.canSeeViewportRenderInfo);

		if (state.lockViewportRenderSize)
		{
			ImGui::SameLine();
			ImGui::Checkbox("Show Render Size", &state.canSeeViewportRenderSize);
			if (state.canSeeViewportRenderSize)
			{
				ImGui::SetNextItemWidth(90.0f);
				ImGui::InputInt("Width", &state.lockedViewportWidth);

				ImGui::SameLine();
				ImGui::SetNextItemWidth(90.0f);
				ImGui::InputInt("Height", &state.lockedViewportHeight);

				ClampViewportSize(state);

				if (ImGui::Button("16:9 1280x720"))
				{
					ApplyViewportPreset(state, 1280, 720);
				}
				ImGui::SameLine();
				if (ImGui::Button("16:9 1920x1080"))
				{
					ApplyViewportPreset(state, 1920, 1080);
				}
				ImGui::SameLine();
				if (ImGui::Button("4:3 1024x768"))
				{
					ApplyViewportPreset(state, 1024, 768);
				}
				ImGui::SameLine();
				if (ImGui::Button("3:2 960x640"))
				{
					ApplyViewportPreset(state, 960, 640);
				}
				ImGui::SameLine();
				if (ImGui::Button("Square 1024x1024"))
				{
					ApplyViewportPreset(state, 1024, 1024);
				}
				ImGui::SameLine();
				if (ImGui::Button("Portrait 720x1280"))
				{
					ApplyViewportPreset(state, 720, 1280);
				}
				ImGui::SameLine();
				if (ImGui::Button("Use Panel Size"))
				{
					state.lockViewportRenderSize = false;
				}
			}
		}

		ImGui::Separator();

		const ImVec2 avail = ImGui::GetContentRegionAvail();

		state.viewportWidth = avail.x;
		state.viewportHeight = avail.y;
		if (state.canSeeViewportRenderInfo)
		{
			ImGui::BeginTable("Info Table", 4, ImGuiTableFlags_Borders | ImGuiTabItemFlags_NoTooltip);
			ImGui::TableHeadersRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Viewport Region: %.0f x %.0f", state.viewportWidth, state.viewportHeight);

			//ImGui::SameLine();
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("Focused: %s", state.scenePreviewSession.focused ? "true" : "false");

			//ImGui::SameLine();
			ImGui::TableSetColumnIndex(2);
			ImGui::Text("Hovered: %s", state.scenePreviewSession.hovered ? "true" : "false");

			//ImGui::SameLine();

			ImGui::TableSetColumnIndex(3);
			if (state.lockViewportRenderSize)
			{
				ImGui::Text("Render Size: %d x %d", state.lockedViewportWidth, state.lockedViewportHeight);
			}
			else
			{
				ImGui::TextUnformatted("Render Size: panel size");
			}

			ImGui::EndTable();
		}

		DrawViewportTexture(
			state.sceneColorTexture,
			state.sceneFramebufferWidth,
			state.sceneFramebufferHeight,
			state.viewportDisplayMode,
			state.viewportWidth,
			state.viewportHeight
		);

		const bool imageAvailable =
			state.sceneColorTexture != 0 &&
			state.sceneFramebufferWidth > 0 &&
			state.sceneFramebufferHeight > 0;

		if (imageAvailable)
		{
			state.scenePreviewSession.hovered = ImGui::IsItemHovered();

			if (!state.scenePreviewSession.path.empty())
			{
				UpdateEditorViewportNavigation(
					state.scenePreviewSession.cameraSettings,
					m_CameraNavigation,
					state.sceneFramebufferWidth,
					state.sceneFramebufferHeight);
			}
			else
			{
				m_CameraNavigation.Reset();
			}
		}
		else
		{
			state.scenePreviewSession.hovered = false;
			m_CameraNavigation.Reset();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload =
				ImGui::AcceptDragDropPayload("PREFAB"))
			{
				const char* droppedPath =
					static_cast<const char*>(payload->Data);

				if (droppedPath && *droppedPath)
				{
					if (IAssetBehavior* behavior =
						state.assetBehaviors.Find(AssetType::Prefab))
					{
						behavior->OnDropped(
							state,
							droppedPath,
							"");
					}
				}
			}

			ImGui::EndDragDropTarget();
		}

		if (!state.scenePreviewSession.path.empty())
		{
			auto& settings = state.scenePreviewSession.cameraSettings;

			settings.displayMode = state.viewportDisplayMode;

			settings.viewportWidth = state.viewportWidth;

			settings.viewportHeight = state.viewportHeight;

			const bool panFinished = ImGui::IsMouseReleased(ImGuiMouseButton_Middle);

			const bool wheelUsed = state.scenePreviewSession.hovered && ImGui::GetIO().MouseWheel != 0.0f;

			if (panFinished || wheelUsed)
			{
				settings.Serialize(state.scenePreviewSession.path);
			}
		}
	}
	ImGui::End();
}
