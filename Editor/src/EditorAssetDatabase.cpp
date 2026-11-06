#include "EditorAssetDatabase.h"

#include "AnimationAsset.h"
#include "Engine.h"
#include "Component.h"
#include "Entity.h"
#include "EditorAssetEntries.h"
#include "InspectorPropertyDrawer.h"

#include <algorithm>
#include <filesystem>
#include <fstream>

#include "nlohmann/json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace buki
{
    namespace
    {
        bool IsAnimFile(const fs::path& path)
        {
            return path.extension() == ".anim";
        }

        bool IsClipFile(const fs::path& path)
        {
            return path.extension() == ".clip";
        }

        std::string CanonicalizePath(const std::string& path)
        {
            return ToAssetRelativePath(
                fs::absolute(path).lexically_normal());
        }
    }

    void EditorAssetDatabase::ScanDirectory(const std::string& root)
    {
        m_Assets.clear();

        const fs::path rootPath =
            fs::absolute(root).lexically_normal();

        if (!fs::exists(rootPath) || !fs::is_directory(rootPath))
            return;

        for (const fs::directory_entry& entry :
            fs::recursive_directory_iterator(rootPath))
        {
            if (!entry.is_regular_file())
                continue;

            const std::string canonicalPath =
                ToAssetRelativePath(
                    fs::absolute(entry.path()).lexically_normal());

            AssetRecord record;
            record.path = canonicalPath;
            record.assetType = GetAssetType(canonicalPath);
            record.id = MakeStableId(canonicalPath);

            if (record.assetType.empty())
                continue;

            m_Assets.push_back(record);
        }

        std::sort(
            m_Assets.begin(),
            m_Assets.end(),
            [](const AssetRecord& a, const AssetRecord& b)
            {
                return a.path < b.path;
            });

        BuildReferences();
    }

    const AssetRecord* EditorAssetDatabase::FindByPath(
        const std::string& path) const
    {
        const std::string canonicalPath =
            ToAssetRelativePath(
                fs::absolute(path).lexically_normal());

        for (const AssetRecord& record : m_Assets)
        {
            if (record.path == canonicalPath)
                return &record;
        }

        return nullptr;
    }

    const AssetRecord* EditorAssetDatabase::FindById(
        EditorId id) const
    {
        for (const AssetRecord& record : m_Assets)
        {
            if (record.id == id)
                return &record;
        }

        return nullptr;
    }

    bool EditorAssetDatabase::RenameAsset(
        const std::string& oldPath,
        const std::string& newPath)
    {
        const std::string oldCanonicalPath =
            ToAssetRelativePath(
                fs::absolute(oldPath).lexically_normal());

        const std::string newCanonicalPath =
            ToAssetRelativePath(
                fs::absolute(newPath).lexically_normal());

        if (oldCanonicalPath == newCanonicalPath)
            return false;

        const fs::path oldAbsolutePath =
            ToAbsolutePath(oldCanonicalPath);

        const fs::path newAbsolutePath =
            ToAbsolutePath(newCanonicalPath);

        if (!fs::exists(oldAbsolutePath))
            return false;

        if (fs::exists(newAbsolutePath))
            return false;

        if (!UpdateReferences(oldCanonicalPath, newCanonicalPath))
            return false;

        std::error_code error;
        fs::rename(oldAbsolutePath, newAbsolutePath, error);

        if (error)
            return false;

        Engine::Get().Assets().Unload(oldCanonicalPath);
        ScanDirectory("../Deployment");
        InspectorPropertyDrawer::RefreshAssetPickers();

        return true;
    }

    bool buki::EditorAssetDatabase::DeleteAsset(
        const std::string& path)
    {
        const std::string canonicalPath =
            ToAssetRelativePath(
                fs::absolute(path).lexically_normal());

        if (!UpdateReferences(canonicalPath, ""))
            return false;

        const fs::path absolutePath =
            ToAbsolutePath(canonicalPath);

        std::error_code error;
        fs::remove(absolutePath, error);

        if (error)
            return false;

        Engine::Get().Assets().Unload(canonicalPath);
        ScanDirectory("../Deployment");
        InspectorPropertyDrawer::RefreshAssetPickers();

        return true;
    }

    bool EditorAssetDatabase::UpdateReferences(
        const std::string& oldPath,
        const std::string& newPath)
    {
        const std::string oldCanonicalPath =
            ToAssetRelativePath(
                fs::absolute(oldPath).lexically_normal());

        const std::string newCanonicalPath =
            newPath.empty()
            ? std::string()
            : ToAssetRelativePath(
                fs::absolute(newPath).lexically_normal());

        const auto referenceIt =
            m_References.find(oldCanonicalPath);

        if (referenceIt == m_References.end())
            return true;

        bool allReferencesUpdated = true;

        for (const std::string& referencingPath :
            referenceIt->second)
        {
            const AssetRecord* record =
                FindByPath(referencingPath);

            if (!record)
                continue;

            bool updated = true;

            if (record->assetType == "ANIM")
            {
                updated = UpdateAnimationReferences(
                    record->path,
                    oldCanonicalPath,
                    newCanonicalPath);
            }
            else if (record->assetType == "PREFAB")
            {
                updated = UpdatePrefabReferences(
                    record->path,
                    oldCanonicalPath,
                    newCanonicalPath);
            }

            if (!updated)
                allReferencesUpdated = false;
        }

        return allReferencesUpdated;
    }

    std::string EditorAssetDatabase::GetAssetType(
        const std::string& path) const
    {
        const fs::path asPath(path);

        if (IsAnimFile(asPath))
            return "ANIM";

        if (IsClipFile(asPath))
            return "CLIP";

        return "";
    }

    bool EditorAssetDatabase::UpdatePrefabReferences(
        const std::string& prefabPath,
        const std::string& oldPath,
        const std::string& newPath)
    {
        IWorld* world = Engine::Get().GetWorldPtr();

        if (!world)
            return false;

        Entity* prefabEntity =
            world->GetOrLoadPrefabEntity(prefabPath);

        if (!prefabEntity)
            return false;

        bool changed = false;

        for (Component* component : prefabEntity->GetComponents())
        {
            if (!component)
                continue;

            const AssetReferenceUpdateResult result =
                component->ReplaceAssetReference(
                    oldPath,
                    newPath);

            if (result == AssetReferenceUpdateResult::Changed)
            {
                changed = true;
            }
            else if (result == AssetReferenceUpdateResult::Failed)
            {
                Engine::Get().Log().LogError(
                    "Failed to update prefab reference: " +
                    prefabPath);
            }
        }

        if (!changed)
            return true;

        return world->SavePrefabAsset(prefabPath);
    }

    void EditorAssetDatabase::CollectPrefabReferences(
        const std::string& prefabPath)
    {
        IWorld* world = Engine::Get().GetWorldPtr();

        if (!world)
            return;

        Entity* prefabEntity =
            world->GetOrLoadPrefabEntity(prefabPath);

        if (!prefabEntity)
            return;

        std::set<std::string> references;

        for (Component* component : prefabEntity->GetComponents())
        {
            if (!component)
                continue;

            component->CollectAssetReferences(references);
        }

        const std::string canonicalPrefabPath =
            ToAssetRelativePath(
                fs::absolute(prefabPath).lexically_normal());

        for (const std::string& reference : references)
        {
            if (reference.empty())
                continue;

            const std::string canonicalReferencePath =
                ToAssetRelativePath(
                    fs::absolute(reference).lexically_normal());

            m_References[canonicalReferencePath].insert(
                canonicalPrefabPath);
        }
    }

    void EditorAssetDatabase::BuildReferences()
    {
        m_References.clear();

        for (const AssetRecord& record : m_Assets)
        {
            if (record.assetType == "ANIM")
            {
                AnimationAsset asset;

                if (!asset.LoadFromFile(record.path))
                    continue;

                for (const AnimationState& state : asset.states)
                {
                    if (state.clipPath.empty())
                        continue;

                    const std::string canonicalClipPath =
                        ToAssetRelativePath(
                            fs::absolute(state.clipPath)
                            .lexically_normal());

                    m_References[canonicalClipPath].insert(
                        record.path);
                }
            }
            else if (record.assetType == "PREFAB")
            {
                CollectPrefabReferences(record.path);
            }
        }
    }

    bool EditorAssetDatabase::UpdateAnimationReferences(
        const std::string& animationPath,
        const std::string& oldPath,
        const std::string& newPath)
    {
        AnimationAsset asset;

        if (!asset.LoadFromFile(animationPath))
            return false;

        bool changed = false;

        for (AnimationState& state : asset.states)
        {
            if (state.clipPath.empty())
                continue;

            const std::string canonicalStateClipPath =
                ToAssetRelativePath(
                    fs::absolute(state.clipPath)
                    .lexically_normal());

            if (canonicalStateClipPath == oldPath)
            {
                state.clipPath = newPath;
                changed = true;
            }
        }

        if (!changed)
            return true;

        return asset.SaveToFile(animationPath);
    }

    EditorId EditorAssetDatabase::MakeStableId(
        const std::string& path)
    {
        // Temporary stable identity until assets persist GUIDs.
        std::uint64_t hash = 1469598103934665603ULL;

        for (const char character : path)
        {
            hash ^= static_cast<std::uint64_t>(
                static_cast<unsigned char>(character));

            hash *= 1099511628211ULL;
        }

        return hash == InvalidEditorId ? 1 : hash;
    }
}