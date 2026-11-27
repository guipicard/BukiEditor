#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include "EditorState.h"
#include "EditorAssetEntries.h"
#include <glad/glad.h>
#include <cmath>
#include <algorithm>

namespace buki
{
    static void DrawViewportTexture(
        unsigned int texture,
        int framebufferWidth,
        int framebufferHeight,
        ViewportDisplayMode mode,
        float& outViewportWidth,
        float& outViewportHeight)
    {
        const ImVec2 available = ImGui::GetContentRegionAvail();
        const ImVec2 regionMin = ImGui::GetCursorScreenPos();

        outViewportWidth = std::max(0.0f, available.x);
        outViewportHeight = std::max(0.0f, available.y);

        if (texture == 0 ||
            framebufferWidth <= 0 ||
            framebufferHeight <= 0 ||
            available.x <= 0.0f ||
            available.y <= 0.0f)
        {
            ImGui::Dummy(ImVec2(outViewportWidth, outViewportHeight));
            return;
        }

        const float textureWidth = static_cast<float>(framebufferWidth);
        const float textureHeight = static_cast<float>(framebufferHeight);

        ImVec2 imageSize = available;

        if (mode == ViewportDisplayMode::Fit)
        {
            const float scale = std::min(available.x / textureWidth, available.y / textureHeight);

            imageSize = ImVec2(textureWidth * scale, textureHeight * scale);
        }
        else if (mode == ViewportDisplayMode::OneToOne)
        {
            const float scale = std::min(
                1.0f,
                std::min(available.x / textureWidth, available.y / textureHeight));

            imageSize = ImVec2(textureWidth * scale, textureHeight * scale);
        }

        ImVec2 imageMin = regionMin;

        if (mode != ViewportDisplayMode::Stretch)
        {
            imageMin.x += (available.x - imageSize.x) * 0.5f;
            imageMin.y += (available.y - imageSize.y) * 0.5f;
        }

        const ImVec2 framebufferScale = ImGui::GetIO().DisplayFramebufferScale;

        const float scaleX = framebufferScale.x > 0.0f ? framebufferScale.x : 1.0f;
        const float scaleY = framebufferScale.y > 0.0f ? framebufferScale.y : 1.0f;

        // Align relative to the host viewport's origin.
        const ImVec2 hostOrigin = ImGui::GetWindowViewport()->Pos;

        const auto snapX = [scaleX, hostOrigin](float value)
            {
                return hostOrigin.x + std::round((value - hostOrigin.x) * scaleX) / scaleX;
            };

        const auto snapY = [scaleY, hostOrigin](float value)
            {
                return hostOrigin.y + std::round((value - hostOrigin.y) * scaleY) / scaleY;
            };

        const ImVec2 imageMax =
        {
            snapX(imageMin.x + imageSize.x),
            snapY(imageMin.y + imageSize.y)
        };

        imageMin.x = snapX(imageMin.x);
        imageMin.y = snapY(imageMin.y);

        imageSize.x = imageMax.x - imageMin.x;
        imageSize.y = imageMax.y - imageMin.y;

        if (imageSize.x <= 0.0f || imageSize.y <= 0.0f)
        {
            ImGui::Dummy(ImVec2(outViewportWidth, outViewportHeight));
            return;
        }

        GLint previousTexture = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);

        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previousTexture));

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        const ImVec2 regionMax =
        {
            regionMin.x + available.x,
            regionMin.y + available.y
        };

        drawList->AddRectFilled(regionMin, regionMax, IM_COL32(18, 18, 22, 255));

        ImGui::SetCursorScreenPos(imageMin);

        ImGui::Image(
            ToImGuiTextureID(texture),
            imageSize,
            ImVec2(0.0f, 1.0f),
            ImVec2(1.0f, 0.0f));

        if (mode != ViewportDisplayMode::Stretch)
        {
            drawList->AddRect(
                ImGui::GetItemRectMin(),
                ImGui::GetItemRectMax(),
                IM_COL32(90, 90, 110, 255));
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