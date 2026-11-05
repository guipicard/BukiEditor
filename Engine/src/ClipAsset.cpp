#include "ClipAsset.h"

#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace buki
{
    static void ToJson(const SpriteFrame& frame, json& out)
    {
        out["path"] = frame.texturePath;
        out["duration"] = frame.duration;
        out["flipX"] = frame.flipX;
        out["flipY"] = frame.flipY;

        out["sourceRect"]["x"] = frame.sourceRect.x;
        out["sourceRect"]["y"] = frame.sourceRect.y;
        out["sourceRect"]["w"] = frame.sourceRect.w;
        out["sourceRect"]["h"] = frame.sourceRect.h;
    }

    static SpriteFrame ParseFrame(const json& frame)
    {
        SpriteFrame result;

        result.texturePath = frame.value("path", "");
        result.duration = frame.value("duration", 0.1f);
        result.flipX = frame.value("flipX", false);
        result.flipY = frame.value("flipY", false);

        if (frame.contains("sourceRect"))
        {
            const json& rect = frame["sourceRect"];

            result.sourceRect.x = rect.value("x", 0.0f);
            result.sourceRect.y = rect.value("y", 0.0f);
            result.sourceRect.w = rect.value("w", 0.0f);
            result.sourceRect.h = rect.value("h", 0.0f);
        }

        return result;
    }

    bool ClipAsset::SaveToFile(const std::string& path) const
    {
        json root;
        root["version"] = version;
        root["name"] = name;
        root["loop"] = loop;

        json framesJson = json::array();

        for (const SpriteFrame& frame : frames)
        {
            json frameJson;
            ToJson(frame, frameJson);
            framesJson.push_back(frameJson);
        }

        root["frames"] = framesJson;

        std::ofstream file(path);
        if (!file.is_open())
            return false;

        file << root.dump(4);
        return true;
    }

    bool ClipAsset::LoadFromFile(const std::string& path)
    {
        const fs::path absolutePath =
            fs::absolute(fs::path("../Deployment") / path).lexically_normal();

        std::ifstream file(absolutePath);
        if (!file.is_open())
            return false;

        json root;

        try
        {
            file >> root;
        }
        catch (const json::exception&)
        {
            return false;
        }

        if (!root.is_object())
            return false;

        version = root.value("version", 1);
        name = root.value("name", "");
        loop = root.value("loop", true);

        frames.clear();

        if (root.contains("frames") && root["frames"].is_array())
        {
            for (const json& frameJson : root["frames"])
                frames.push_back(ParseFrame(frameJson));
        }

        return true;
    }
}