#include "AssetManager.h"

#include <algorithm>
#include <filesystem>

namespace buki
{
    void AssetManager::SetRoot(const std::string& root)
    {
        m_Root = std::filesystem::absolute(root)
            .lexically_normal()
            .generic_string();
    }

    const std::string& AssetManager::GetRoot() const
    {
        return m_Root;
    }

    std::string AssetManager::ResolvePath(
        const std::string& path) const
    {
        const std::filesystem::path asPath(path);

        if (asPath.is_absolute())
        {
            return asPath.lexically_normal().generic_string();
        }

        return (std::filesystem::path(m_Root) / asPath)
            .lexically_normal()
            .generic_string();
    }

    void AssetManager::Unload(const std::string& path)
    {
        m_Assets.erase(ResolvePath(path));
    }

    void AssetManager::Clear()
    {
        m_Assets.clear();
    }
}