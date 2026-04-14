#pragma once

#include <cstdint>
#include <string>

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
    };

    namespace TextureLoader
    {
        Texture2D LoadFromFile(IGraphics& graphics, const std::string& path, bool flipVertically = true);
        void Destroy(IGraphics& graphics, Texture2D& texture);
    }
}
