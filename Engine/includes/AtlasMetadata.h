#pragma once

#include "Graphics/Texture2D.h"
#include "BukiContainers.h"

#include <string>
#include <map>

namespace buki
{
    struct AtlasFrame
    {
        std::string name;
        RectF sourceRect{};

        int sourceWidth = 0;
        int sourceHeight = 0;

        int spriteSourceX = 0;
        int spriteSourceY = 0;
        int spriteSourceW = 0;
        int spriteSourceH = 0;

        float duration = 0.1f;

        bool rotated = false;
        bool trimmed = false;

        float pivotX = 0.5f;
        float pivotY = 0.5f;
    };

    class AtlasMetadata
    {
    public:
        AtlasMetadata() = default;
        ~AtlasMetadata() = default;

        bool LoadAsepriteJson(const std::string& jsonPath);

        void Clear();

        [[nodiscard]] const AtlasFrame* GetFrame(const std::string& name) const;
        [[nodiscard]] bool HasFrame(const std::string& name) const;

    private:
        std::map<std::string, AtlasFrame> m_Frames;
    };
}
