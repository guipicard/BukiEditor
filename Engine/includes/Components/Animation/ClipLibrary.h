#pragma once

#include "AnimationTypes.h"
#include "ClipAsset.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace buki
{
    class ClipLibrary
    {
    public:
        ClipLibrary() = default;
        ~ClipLibrary() = default;

        ClipAsset* GetOrLoad(const std::string& clipPath);
        ClipAsset* Find(const std::string& clipPath);
        void Unload(const std::string& clipPath);
        void Clear();

    private:
        std::unordered_map<std::string, std::unique_ptr<ClipAsset>> m_Clips;
    };

    class AsepriteClipImporter
    {
    public:
        static bool Import(
            const std::string& texturePath,
            const std::string& atlasPath,
            const std::string& clipJsonPath,
            ClipAsset& outClip);
    };
}