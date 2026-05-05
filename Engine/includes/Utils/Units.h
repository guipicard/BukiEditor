#pragma once

#include <glm/vec2.hpp>

namespace buki
{
    struct Units
    {
        static constexpr float PixelsPerMeter = 50.0f;
        static constexpr float MetersPerPixel = 1.0f / PixelsPerMeter;

        static constexpr float ToPixels(float meters)
        {
            return meters * PixelsPerMeter;
        }

        static constexpr float ToMeters(float pixels)
        {
            return pixels * MetersPerPixel;
        }

        static constexpr glm::vec2 ToPixels(const glm::vec2& meters)
        {
            return { meters.x * PixelsPerMeter, meters.y * PixelsPerMeter };
        }

        static constexpr glm::vec2 ToMeters(const glm::vec2& pixels)
        {
            return { pixels.x * MetersPerPixel, pixels.y * MetersPerPixel };
        }
    };
}