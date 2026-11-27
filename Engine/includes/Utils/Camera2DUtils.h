#pragma once

#include "Graphics/Camera2D.h"
#include "Units.h"

namespace buki
{
    inline glm::vec2 WorldToScreen(const glm::vec2& worldPositionMeters, const Camera2D& camera)
    {
        const glm::vec2 relativeWorld = worldPositionMeters - camera.position;
        const glm::vec2 relativePixels = Units::ToPixels(relativeWorld);

        const glm::vec2 screenOffset =
        {
            relativePixels.x * camera.zoom,
            -relativePixels.y * camera.zoom
        };

        return camera.GetViewportCenter() + screenOffset;
    }

    inline float WorldToScreenSize(float sizeMeters, const Camera2D& camera)
    {
        return Units::ToPixels(sizeMeters) * camera.zoom;
    }

    inline glm::vec2 ScreenToWorld(const glm::vec2& screenPositionPixels, const Camera2D& camera)
    {
        const glm::vec2 relativePixels = (screenPositionPixels - camera.GetViewportCenter()) / camera.zoom;

        const glm::vec2 worldPixels =
        {
            relativePixels.x,
            -relativePixels.y
        };

        return camera.position + Units::ToMeters(worldPixels);
    }

    inline float WorldToScreenRotation(float rotationRadians)
    {
        return -rotationRadians;
    }
}