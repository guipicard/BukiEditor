#pragma once
#include "BukiContainers.h"
#include <glm/glm.hpp>

namespace buki
{
    struct Camera2D
    {
        Vector2 position{ 0.0f, 0.0f };
        float rotation = 0.0f;
        float zoom = 1.0f;
        float viewportWidth = 1280.0f;
        float viewportHeight = 720.0f;

        glm::mat4 GetViewProjection() const;
    };

}
