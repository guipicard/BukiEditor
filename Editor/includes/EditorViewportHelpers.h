#include "imgui.h"
#include "imgui_internal.h"
#include "EditorState.h"
#include "EditorAssetEntries.h"

namespace buki
{
	static void DrawViewportTexture(unsigned int texture, int framebufferWidth, int framebufferHeight, ViewportDisplayMode mode, float& outViewportWidth, float& outViewportHeight)
	{
		const ImVec2 imageRegionStart = ImGui::GetCursorPos();
		const ImVec2 imageAvail = ImGui::GetContentRegionAvail();

		outViewportWidth = imageAvail.x;
		outViewportHeight = imageAvail.y;

		if (texture == 0 || imageAvail.x <= 0.0f || imageAvail.y <= 0.0f)
		{
			ImGui::TextUnformatted("Viewport texture unavailable.");
			ImGui::Dummy(ImVec2(imageAvail.x, imageAvail.y > 0.0f ? imageAvail.y : 0.0f));
			return;
		}

		const float texW = static_cast<float>(std::max(1, framebufferWidth));
		const float texH = static_cast<float>(std::max(1, framebufferHeight));

		ImVec2 imageSize = imageAvail;
		ImVec2 cursor = imageRegionStart;

		if (mode == ViewportDisplayMode::Fit)
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
		else if (mode == ViewportDisplayMode::OneToOne)
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
			ToImGuiTextureID(texture),
			imageSize,
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		if (mode != ViewportDisplayMode::Stretch)
		{
			drawList->AddRect(
				ImGui::GetItemRectMin(),
				ImGui::GetItemRectMax(),
				IM_COL32(90, 90, 110, 255)
			);
		}
	}

	static void DrawPrefabSessionTexture(PrefabPreviewSession& session)
	{
		DrawViewportTexture(
			session.colorTexture,
			session.framebufferWidth,
			session.framebufferHeight,
			session.cameraSettings.displayMode,
			session.cameraSettings.viewportWidth,
			session.cameraSettings.viewportHeight
		);
	}
}