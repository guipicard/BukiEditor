#pragma once
#include "AssetBehaviorRegistry.h"

#include "IAssetBehavior.h"
#include "EditorAssetEntries.h"
#include "AssetType.h"

#include "EditorState.h"

#include <filesystem>

namespace buki
{
    void AssetBehaviorRegistry::Register(
        AssetType type,
        std::unique_ptr<IAssetBehavior> behavior)
    {
        if (!behavior)
            return;

        m_Behaviors.insert_or_assign(
            type,
            std::move(behavior));
    }

    IAssetBehavior* AssetBehaviorRegistry::Find(
        AssetType type) const
    {
        const auto it = m_Behaviors.find(type);

        if (it == m_Behaviors.end())
            return nullptr;

        return it->second.get();
    }

    IAssetBehavior* AssetBehaviorRegistry::FindByPath(
        const std::string& path) const
    {
        return Find(GetAssetType(path));
    }

    AssetType AssetBehaviorRegistry::GetAssetType(
        const std::string& path) const
    {
        const std::string extension =
            ToLowerCopy(
                fs::path(path).extension().string());

        if (extension == ".anim")
            return AssetType::Anim;

        if (extension == ".clip")
            return AssetType::Clip;

        if (extension == ".prefab")
            return AssetType::Prefab;

        if (extension == ".png" || extension == ".jpg" ||
            extension == ".jpeg" || extension == ".bmp" ||
            extension == ".tga" || extension == ".gif" ||
            extension == ".webp")
        {
            return AssetType::Image;
        }

        if (extension == ".wav" || extension == ".mp3" ||
            extension == ".ogg" || extension == ".flac")
        {
            return AssetType::Audio;
        }

        if (extension == ".json" || extension == ".scene")
            return AssetType::Scene;

        return AssetType::File;
    }
}