#include "ClipLibrary.h"
#include "Engine.h"

namespace buki
{
    ClipAsset* ClipLibrary::GetOrLoad(const std::string& clipPath)
    {
        if (clipPath.empty())
            return nullptr;

        auto it = m_Clips.find(clipPath);
        if (it != m_Clips.end())
            return it->second.get();

        auto clip = std::make_unique<ClipAsset>();

        if (!clip->LoadFromFile(clipPath))
            return nullptr;

        for (SpriteFrame& frame : clip->frames)
        {
            if (frame.texturePath.empty())
                continue;

            frame.texture = Engine::Get().Textures().Load(frame.texturePath);
        }

        ClipAsset* result = clip.get();
        m_Clips[clipPath] = std::move(clip);
        return result;
    }

    ClipAsset* ClipLibrary::Find(const std::string& clipPath)
    {
        auto it = m_Clips.find(clipPath);
        return it != m_Clips.end() ? it->second.get() : nullptr;
    }

    void ClipLibrary::Unload(const std::string& clipPath)
    {
        m_Clips.erase(clipPath);
    }

    void ClipLibrary::Clear()
    {
        m_Clips.clear();
    }
}