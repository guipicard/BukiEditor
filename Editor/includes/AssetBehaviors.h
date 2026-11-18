#pragma once

#include "IAssetBehavior.h"

#include "AssetType.h"

namespace buki
{
    class EditorAssetDatabase;
    class AssetBehaviorBase : public IAssetBehavior
    {
    public:
        void OnReferenceChanged(
            EditorAssetDatabase& database,
            const std::string& ownerAssetPath,
            const std::string& oldReferencedPath,
            const std::string& newReferencedPath) override;

        bool UpdateReferences(
            EditorAssetDatabase& database,
            const std::string& referencingAssetPath,
            const std::string& oldReferencedPath,
            const std::string& newReferencedPath) override;
    };


    class ClipAssetBehavior : public AssetBehaviorBase
    {
    public:
        [[nodiscard]] AssetType GetType() const override
        {
            return AssetType::Clip;
        }

        [[nodiscard]] const char* GetPayloadType() const override
        {
            return "CLIP";
        }

        bool Create(
            const std::string& directory,
            const std::string& baseName,
            std::string& outPath) override;

        void OnDoubleClick(
            EditorState& state,
            const BrowserEntry& entry) override;

        void OnDropped(
            EditorState& state,
            const std::string& assetPath,
            const std::string& targetPath) override;

        bool UpdateReferences(
            EditorAssetDatabase& database,
            const std::string& referencingAssetPath,
            const std::string& oldReferencedPath,
            const std::string& newReferencedPath) override;

        void CollectReferences(
            EditorAssetDatabase& database,
            const std::string& path,
            std::vector<std::string>& outReferences) override;
    };

    class AnimationAssetBehavior : public AssetBehaviorBase
    {
    public:
        [[nodiscard]] AssetType GetType() const override
        {
            return AssetType::Anim;
        }

        [[nodiscard]] const char* GetPayloadType() const override
        {
            return "ANIM";
        }

        bool Create(
            const std::string& directory,
            const std::string& baseName,
            std::string& outPath) override;

        void OnDoubleClick(
            EditorState& state,
            const BrowserEntry& entry) override;

        void OnDropped(
            EditorState& state,
            const std::string& assetPath,
            const std::string& targetPath) override;

        bool UpdateReferences(
            EditorAssetDatabase& database,
            const std::string& referencingAssetPath,
            const std::string& oldReferencedPath,
            const std::string& newReferencedPath) override;

        void CollectReferences(
            EditorAssetDatabase& database,
            const std::string& path,
            std::vector<std::string>& outReferences) override;
    };

    class PrefabAssetBehavior : public AssetBehaviorBase
    {
    public:
        [[nodiscard]] AssetType GetType() const override
        {
            return AssetType::Prefab;
        }

        [[nodiscard]] const char* GetPayloadType() const override
        {
            return "PREFAB";
        }

        bool Create(
            const std::string& directory,
            const std::string& baseName,
            std::string& outPath) override;

        void OnDoubleClick(
            EditorState& state,
            const BrowserEntry& entry) override;

        void OnDropped(
            EditorState& state,
            const std::string& assetPath,
            const std::string& targetPath) override;

        bool UpdateReferences(
            EditorAssetDatabase& database,
            const std::string& referencingAssetPath,
            const std::string& oldReferencedPath,
            const std::string& newReferencedPath) override;

        void CollectReferences(
            EditorAssetDatabase& database,
            const std::string& path,
            std::vector<std::string>& outReferences) override;

    };

    class SceneAssetBehavior : public AssetBehaviorBase
    {
    public:
        [[nodiscard]] AssetType GetType() const override
        {
            return AssetType::Scene;
        }

        [[nodiscard]] const char* GetPayloadType() const override
        {
            return "SCENE";
        }

        bool Create(
            const std::string& directory,
            const std::string& baseName,
            std::string& outPath) override
        {
            return false;
        }

        void OnDoubleClick(
            EditorState& state,
            const BrowserEntry& entry) override;

        void OnDropped(
            EditorState& state,
            const std::string& assetPath,
            const std::string& targetPath) override;
        
        bool UpdateReferences(
            EditorAssetDatabase& database,
            const std::string& referencingAssetPath,
            const std::string& oldReferencedPath,
            const std::string& newReferencedPath) override;

        void CollectReferences(
            EditorAssetDatabase& database,
            const std::string& path,
            std::vector<std::string>& outReferences) override;
    };
}