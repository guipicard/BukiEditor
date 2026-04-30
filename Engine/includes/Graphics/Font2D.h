#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include "BukiContainers.h"

namespace buki
{
    struct Glyph2D
    {
        RectF sourceRectPixels{};
        int width = 0;
        int height = 0;

        int bearingX = 0;
        int bearingY = 0;
        int advance = 0;
    };

    struct Font2D
    {
        std::uint32_t textureId = 0;
        int atlasWidth = 0;
        int atlasHeight = 0;
        int fontSize = 0;
        int lineHeight = 0;
        bool valid = false;

        std::unordered_map<char, Glyph2D> glyphs;

        [[nodiscard]] bool IsValid() const
        {
            return valid && textureId != 0 && atlasWidth > 0 && atlasHeight > 0;
        }

        [[nodiscard]] const Glyph2D* GetGlyph(char c) const
        {
            auto it = glyphs.find(c);
            if (it == glyphs.end())
            {
                return nullptr;
            }
            return &it->second;
        }
    };
}