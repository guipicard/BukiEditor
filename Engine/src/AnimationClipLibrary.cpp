#pragma once
#include "AnimationClipLibrary.h"
#include "Animation.h"
#include "AtlasMetadata.h"
#include "Engine.h"
#include "nlohmann/json.hpp"

#include <fstream>

using json = nlohmann::json;

namespace buki
{
    AnimationClipLibrary::AnimationClipLibrary()
    {
        m_Clips = std::map<std::string, AnimationClip>();
    }
	bool AnimationClipLibrary::LoadFromFile(const std::string& clipJsonPath)
	{
		Clear();

		std::ifstream file(clipJsonPath);
		if (!file.is_open())
		{
			return false;
		}

		json doc;
		file >> doc;

		const std::string texturePath = doc.value("texture", "");
		const std::string atlasPath = doc.value("atlas", "");

		if (texturePath.empty() || atlasPath.empty() || !doc.contains("clips"))
		{
			return false;
		}

		Texture2D* texture = Engine::Get().Textures().Load(texturePath);
		if (texture == nullptr)
		{
			return false;
		}

		AtlasMetadata atlas;
		if (!atlas.LoadAsepriteJson(atlasPath))
		{
			return false;
		}

		const auto& clipsDoc = doc["clips"];
		for (auto it = clipsDoc.begin(); it != clipsDoc.end(); ++it)
		{
			AnimationClip clip;
			clip.name = it.key();
			clip.loop = it.value().value("loop", true);

			if (!it.value().contains("frames") || !it.value()["frames"].is_array())
			{
				continue;
			}

			for (const auto& frameRef : it.value()["frames"])
			{
				const std::string frameName = frameRef.value("frame", "");
				if (frameName.empty())
				{
					continue;
				}

				const AtlasFrame* atlasFrame = atlas.GetFrame(frameName);
				if (atlasFrame == nullptr)
				{
					continue;
				}

				SpriteFrame spriteFrame;
				spriteFrame.texture = texture;
				spriteFrame.sourceRect = atlasFrame->sourceRect;
				spriteFrame.duration = frameRef.value("duration", atlasFrame->duration);

				clip.frames.push_back(spriteFrame);
			}

			if (!clip.frames.empty())
			{
				m_Clips[clip.name] = clip;
			}
		}

		return !m_Clips.empty();
	}

    void AnimationClipLibrary::AddClip(const AnimationClip& clip)
    {
		m_Clips[clip.name] = clip;
    }

    const AnimationClip* AnimationClipLibrary::GetClip(const std::string& name) const
    {
        auto it = m_Clips.find(name);
        if (it == m_Clips.end())
        {
            return nullptr;
        }

        return &it->second;
    }

    void AnimationClipLibrary::Clear()
    {
        m_Clips.clear();
    }
}
