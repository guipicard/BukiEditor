#pragma once

#include "AnimationTypes.h"
#include "AtlasMetadata.h"

#include <string>
#include <map>

namespace buki
{
    class AnimationClipLibrary
    {
    public:
        AnimationClipLibrary();
        ~AnimationClipLibrary() = default;

        bool LoadFromFile(const std::string& clipJsonPath);
		void AddClip(const AnimationClip& clip);

        [[nodiscard]] const AnimationClip* GetClip(const std::string& name) const;
        [[nodiscard]] const std::map<std::string, AnimationClip>& GetClips() const { return m_Clips; }

        void Clear();

    private:
        std::map<std::string, AnimationClip> m_Clips;
    };
}
