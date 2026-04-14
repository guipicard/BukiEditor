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
        float originX = 0.0f;
        float originY = 0.0f;
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