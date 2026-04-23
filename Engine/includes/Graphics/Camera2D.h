#pragma once

#include <glm/vec2.hpp>

namespace buki
{
    struct Camera2D
    {
        glm::vec2 position{ 0.0f, 0.0f }; // meters
        float zoom = 1.0f;

        float viewportWidth = 1280.0f;   // pixels
        float viewportHeight = 720.0f;   // pixels

        [[nodiscard]] glm::vec2 GetViewportCenter() const
        {
            return { viewportWidth * 0.5f, viewportHeight * 0.5f };
        }
    };
}