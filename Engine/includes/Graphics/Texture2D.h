#pragma once

#include <cstdint>
#include <string>
#include "BukiContainers.h"

namespace buki
{
    class IGraphics;

    struct Texture2D
    {
        std::uint32_t id = 0;
        int width = 0;
        int height = 0;
        bool valid = false;

        [[nodiscard]] bool IsValid() const
        {
            return valid && id != 0 && width > 0 && height > 0;
        }
        Vector2 SizePixels() const
        {
            return { static_cast<float>(width), static_cast<float>(height) };
        }

        Vector2 SizeMeters() const
        {
            return { width / METRES_TO_PIXELS, height / METRES_TO_PIXELS };
        }
    };

    namespace TextureLoader
    {
        Texture2D LoadFromFile(IGraphics& graphics, const std::string& path);
        void Destroy(IGraphics& graphics, Texture2D& texture);
    }
}
