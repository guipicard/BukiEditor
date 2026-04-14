#include "AtlasMetadata.h"

#include "nlohmann/json.hpp"

#include <fstream>

using json = nlohmann::json;

namespace buki
{
    namespace
    {
        AtlasFrame ParseFrame(const std::string& name, const json& frameDoc)
        {
            AtlasFrame frame;
            frame.name = name;

            if (frameDoc.contains("frame"))
            {
                const auto& src = frameDoc["frame"];
                frame.sourceRect.x = src.value("x", 0.0f);
                frame.sourceRect.y = src.value("y", 0.0f);
                frame.sourceRect.w = src.value("w", 0.0f);
                frame.sourceRect.h = src.value("h", 0.0f);
            }

            if (frameDoc.contains("spriteSourceSize"))
            {
                const auto& sss = frameDoc["spriteSourceSize"];
                frame.spriteSourceX = sss.value("x", 0);
                frame.spriteSourceY = sss.value("y", 0);
                frame.spriteSourceW = sss.value("w", 0);
                frame.spriteSourceH = sss.value("h", 0);
            }

            if (frameDoc.contains("sourceSize"))
            {
                const auto& ss = frameDoc["sourceSize"];
                frame.sourceWidth = ss.value("w", 0);
                frame.sourceHeight = ss.value("h", 0);
            }

            if (frameDoc.contains("pivot"))
            {
                const auto& pivot = frameDoc["pivot"];
                frame.pivotX = pivot.value("x", 0.5f);
                frame.pivotY = pivot.value("y", 0.5f);
            }

            frame.duration = frameDoc.value("duration", 100.0f) / 1000.0f;
            frame.rotated = frameDoc.value("rotated", false);
            frame.trimmed = frameDoc.value("trimmed", false);

            return frame;
        }
    }

    bool AtlasMetadata::LoadAsepriteJson(const std::string& jsonPath)
    {
        Clear();

        std::ifstream file(jsonPath);
        if (!file.is_open())
        {
            return false;
        }

        json doc;
        file >> doc;

        if (!doc.contains("frames"))
        {
            return false;
        }

        const auto& frames = doc["frames"];

        if (frames.is_array())
        {
            for (const auto& entry : frames)
            {
                std::string name = entry.value("filename", "");
                if (name.empty())
                {
                    continue;
                }

                m_Frames[name] = ParseFrame(name, entry);
            }
        }
        else if (frames.is_object())
        {
            for (auto it = frames.begin(); it != frames.end(); ++it)
            {
                const std::string name = it.key();
                m_Frames[name] = ParseFrame(name, it.value());
            }
        }

        return !m_Frames.empty();
    }

    void AtlasMetadata::Clear()
    {
        m_Frames.clear();
    }

    const AtlasFrame* AtlasMetadata::GetFrame(const std::string& name) const
    {
        auto it = m_Frames.find(name);
        if (it == m_Frames.end())
        {
            return nullptr;
        }

        return &it->second;
    }

    bool AtlasMetadata::HasFrame(const std::string& name) const
    {
        return m_Frames.find(name) != m_Frames.end();
    }
}
