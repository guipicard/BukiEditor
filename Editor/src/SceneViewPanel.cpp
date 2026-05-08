#include "SceneViewPanel.h"

#include "imgui.h"
#include "Engine.h"
#include <cstdint>
#include <algorithm>

#include <fstream>

namespace fs = std::filesystem;

namespace
{
	static ImTextureID ToImTextureID(unsigned int texture)
	{
		return (ImTextureID)(intptr_t)(texture);
	}

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
	ImGui::Begin("Scene", &state.showSceneView);

	state.viewportFocused = ImGui::IsWindowFocused();
	state.viewportHovered = ImGui::IsWindowHovered();

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

	const ImVec2 viewportRegionMin = ImGui::GetCursorScreenPos();
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
		ImGui::Text("Focused: %s", state.viewportFocused ? "true" : "false");

		//ImGui::SameLine();
		ImGui::TableSetColumnIndex(2);
		ImGui::Text("Hovered: %s", state.viewportHovered ? "true" : "false");

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
	//ImGui::Separator();

	const ImVec2 imageRegionStart = ImGui::GetCursorPos();
	const ImVec2 imageAvail = ImGui::GetContentRegionAvail();

	if (state.sceneColorTexture != 0 && imageAvail.x > 0.0f && imageAvail.y > 0.0f)
	{
		const float texW = static_cast<float>(std::max(1, state.sceneFramebufferWidth));
		const float texH = static_cast<float>(std::max(1, state.sceneFramebufferHeight));

		ImVec2 imageSize = imageAvail;
		ImVec2 cursor = imageRegionStart;

		if (state.viewportDisplayMode == ViewportDisplayMode::Fit)
		{
			const float textureAspect = texW / texH;
			const float availAspect = imageAvail.x / imageAvail.y;

			if (availAspect > textureAspect)
			{
				imageSize.y = imageAvail.y;
				imageSize.x = imageSize.y * textureAspect;
				cursor.x += (imageAvail.x - imageSize.x) * 0.5f;
			}
			else
			{
				imageSize.x = imageAvail.x;
				imageSize.y = imageSize.x / textureAspect;
				cursor.y += (imageAvail.y - imageSize.y) * 0.5f;
			}
		}
		else if (state.viewportDisplayMode == ViewportDisplayMode::OneToOne)
		{
			imageSize.x = texW;
			imageSize.y = texH;

			if (imageSize.x > imageAvail.x)
			{
				const float scale = imageAvail.x / imageSize.x;
				imageSize.x *= scale;
				imageSize.y *= scale;
			}

			if (imageSize.y > imageAvail.y)
			{
				const float scale = imageAvail.y / imageSize.y;
				imageSize.x *= scale;
				imageSize.y *= scale;
			}

			cursor.x += (imageAvail.x - imageSize.x) * 0.5f;
			cursor.y += (imageAvail.y - imageSize.y) * 0.5f;
		}

		if (imageSize.x < 1.0f) imageSize.x = 1.0f;
		if (imageSize.y < 1.0f) imageSize.y = 1.0f;

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		const ImVec2 contentMin = ImGui::GetCursorScreenPos();
		const ImVec2 contentMax = ImVec2(contentMin.x + imageAvail.x, contentMin.y + imageAvail.y);

		drawList->AddRectFilled(contentMin, contentMax, IM_COL32(18, 18, 22, 255));

		ImGui::SetCursorPos(cursor);
		ImGui::Image(
			ToImTextureID(state.sceneColorTexture),
			imageSize,
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PREFAB"))
			{
				const char* droppedPath = static_cast<const char*>(payload->Data);
				if (droppedPath != nullptr)
				{
					Entity* entity = buki::Engine::Get().World().InstantiatePrefab(fs::path(droppedPath).string());
					if (entity != nullptr)
					{
						// optional: place at camera/world mouse position here
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (state.viewportDisplayMode != ViewportDisplayMode::Stretch)
		{
			drawList->AddRect(
				ImGui::GetItemRectMin(),
				ImGui::GetItemRectMax(),
				IM_COL32(90, 90, 110, 255)
			);
		}
	}
	else
	{
		ImGui::TextUnformatted("Viewport texture unavailable.");
		ImGui::Dummy(ImVec2(imageAvail.x, imageAvail.y > 0.0f ? imageAvail.y : 0.0f));
	}

	ImGui::End();
}