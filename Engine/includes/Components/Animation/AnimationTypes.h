#pragma once

#include "Graphics/Texture2D.h"
#include "BukiContainers.h"

#include <string>
#include <vector>

namespace buki
{
    struct SpriteFrame
    {
        Texture2D* texture = nullptr;
        RectF sourceRect{};
        float duration = 0.1f;
		bool flipX = false;
		bool flipY = false;
    };

    struct AnimationClip
    {
        std::string name;
        std::vector<SpriteFrame> frames;
        bool loop = true;
    };
}