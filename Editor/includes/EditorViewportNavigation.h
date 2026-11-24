#pragma once

#include <algorithm>
#include <cmath>

#include "imgui.h"

#include "Camera2D.h"
#include "Camera2DUtils.h"
#include "EditorCanvasNavigation.h"
#include "EditorPreviewSessions.h"

namespace buki
{
    inline bool UpdateEditorViewportNavigation(
        CameraSettings& settings,
        EditorCanvasNavigation& navigation,
        int framebufferWidth,
        int framebufferHeight)
    {
        // Call immediately after drawing the viewport image.
        const bool imageHovered = ImGui::IsItemHovered();

        const ImVec2 imageMin = ImGui::GetItemRectMin();
        const ImVec2 imageSize = ImGui::GetItemRectSize();

        const ImVec2 mouseDelta =
            navigation.Update(imageHovered);

        if (framebufferWidth <= 0 ||
            framebufferHeight <= 0 ||
            imageSize.x <= 0.0f ||
            imageSize.y <= 0.0f)
        {
            return false;
        }

        Camera2D camera{};

        camera.position =
        {
            settings.position.x,
            settings.position.y
        };

        camera.zoom =
            std::clamp(settings.zoom, 0.05f, 32.0f);

        camera.viewportWidth =
            static_cast<float>(framebufferWidth);

        camera.viewportHeight =
            static_cast<float>(framebufferHeight);

        const float framebufferScaleX =
            camera.viewportWidth / imageSize.x;

        const float framebufferScaleY =
            camera.viewportHeight / imageSize.y;

        bool changed = false;

        if (navigation.panning &&
            (mouseDelta.x != 0.0f ||
                mouseDelta.y != 0.0f))
        {
            const glm::vec2 center =
            {
                camera.viewportWidth * 0.5f,
                camera.viewportHeight * 0.5f
            };

            const glm::vec2 framebufferDelta =
            {
                mouseDelta.x * framebufferScaleX,
                mouseDelta.y * framebufferScaleY
            };

            const glm::vec2 worldBefore =
                ScreenToWorld(center, camera);

            const glm::vec2 worldAfter =
                ScreenToWorld(
                    center + framebufferDelta,
                    camera);

            camera.position -= worldAfter - worldBefore;

            changed = true;
        }

        const float wheel =
            imageHovered ? ImGui::GetIO().MouseWheel : 0.0f;

        if (wheel != 0.0f)
        {
            const ImVec2 mousePosition =
                ImGui::GetIO().MousePos;

            const glm::vec2 mouseInFramebuffer =
            {
                (mousePosition.x - imageMin.x) *
                    framebufferScaleX,

                (mousePosition.y - imageMin.y) *
                    framebufferScaleY
            };

            const glm::vec2 worldBefore =
                ScreenToWorld(mouseInFramebuffer, camera);

            camera.zoom = std::clamp(
                camera.zoom * std::pow(1.15f, wheel),
                0.05f,
                32.0f);

            const glm::vec2 worldAfter =
                ScreenToWorld(mouseInFramebuffer, camera);

            camera.position += worldBefore - worldAfter;

            changed = true;
        }

        if (changed)
        {
            settings.position =
            {
                camera.position.x,
                camera.position.y
            };

            settings.zoom = camera.zoom;
        }

        return changed;
    }
}