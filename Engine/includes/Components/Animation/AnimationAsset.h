#pragma once

#include "AnimationTypes.h"

#include <string>
#include <vector>

namespace buki
{
    struct AnimationStateLayout
    {
        std::string stateName;
        Vector2 editorPosition{};
    };

    struct AnimationClipReference
    {
        std::string clipPath;
    };

    struct AnimationAsset
    {
        int version = 1;
        std::string name;
        std::string initialState;

        std::vector<AnimParameter> parameters;
        std::vector<AnimationState> states;

        std::vector<AnimationStateLayout> layout;

        bool LoadFromFile(const std::string& path);
        bool SaveToFile(const std::string& path) const;

        [[nodiscard]] const AnimationState* GetState(const std::string& name) const;
        [[nodiscard]] const AnimationClipReference* GetClipReference(const std::string& clipPath) const;
        [[nodiscard]] const AnimParameter* GetParameter(const std::string& name) const;
    };
}