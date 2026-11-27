#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

#include <glm/vec2.hpp>

namespace buki
{
    inline std::vector<glm::vec2> MakeRegularPolygonVertices(
        float radius,
        int segments,
        float rotationRadians = 0.0f)
    {
        constexpr float fullTurn = 6.28318530718f;
        constexpr float quarterTurn = fullTurn * 0.25f;

        segments = std::max(segments, 3);

        std::vector<glm::vec2> vertices;
        vertices.reserve(static_cast<std::size_t>(segments));

        const float step = fullTurn / static_cast<float>(segments);

        for (int i = 0; i < segments; ++i)
        {
            const float angle = quarterTurn + rotationRadians + static_cast<float>(i) * step;

            vertices.push_back(
                {
                    std::cos(angle) * radius,
                    std::sin(angle) * radius
                });
        }

        return vertices;
    }
}