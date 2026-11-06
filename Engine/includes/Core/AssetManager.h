#pragma once

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

namespace buki
{
    class AssetManager
    {
    public:
        AssetManager() = default;
        ~AssetManager() = default;

        AssetManager(const AssetManager&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;

        void SetRoot(const std::string& root);
        [[nodiscard]] const std::string& GetRoot() const;

        [[nodiscard]] std::string ResolvePath(
            const std::string& path) const;

        template <typename T>
        [[nodiscard]] T* Get(const std::string& path);

        template <typename T>
        T* GetOrLoad(const std::string& path);

        template <typename T>
        [[nodiscard]] T* FindByFileName(const std::string& fileName);

        void Unload(const std::string& path);
        void Clear();

    private:
        std::string m_Root = "../Deployment";

        std::unordered_map<std::string, std::shared_ptr<void>> m_Assets;
    };

    template <typename T>
    T* AssetManager::Get(const std::string& path)
    {
        const auto it = m_Assets.find(ResolvePath(path));

        if (it == m_Assets.end())
            return nullptr;

        return std::static_pointer_cast<T>(it->second).get();
    }

    template <typename T>
    T* AssetManager::GetOrLoad(const std::string& path)
    {
        const std::string resolvedPath = ResolvePath(path);

        if (T* existing = Get<T>(resolvedPath))
            return existing;

        auto asset = std::make_shared<T>();

        if (!asset->LoadFromFile(resolvedPath))
            return nullptr;

        m_Assets[resolvedPath] = asset;

        return asset.get();
    }

    template <typename T>
    T* AssetManager::FindByFileName(const std::string& fileName)
    {
        const std::string target =
            fs::path(fileName)
            .filename()
            .generic_string();

        for (const auto& [path, asset] : m_Assets)
        {
            const std::string currentFileName =
                std::filesystem::path(path)
                .filename()
                .generic_string();

            if (currentFileName == target)
            {
                return std::static_pointer_cast<T>(asset).get();
            }
        }

        return nullptr;
    }
}