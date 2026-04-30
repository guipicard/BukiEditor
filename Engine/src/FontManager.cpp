#include "Graphics/FontManager.h"

#include "Graphics/IGraphics.h"

#include <filesystem>

namespace buki
{
    FontManager::FontManager(IGraphics& graphics)
        : m_Graphics(graphics)
    {
    }

    FontManager::~FontManager()
    {
        Clear();
    }

    std::string FontManager::BuildAssetPath(const std::string& path) const
    {
#if _DEBUG
        const std::string root = std::filesystem::absolute("../Deployment").string();
#else
        const std::string root = std::filesystem::absolute(".").string();
#endif
        return root + path;
    }

    std::string FontManager::BuildKey(const std::string& path, int fontSize) const
    {
        return BuildAssetPath(path) + "#" + std::to_string(fontSize);
    }

    Font2D* FontManager::Load(const std::string& path, int fontSize)
    {
        const std::string key = BuildKey(path, fontSize);

        auto it = m_Fonts.find(key);
        if (it != m_Fonts.end())
        {
            return &it->second;
        }

        Font2D font = m_Graphics.CreateFontFromFile(BuildAssetPath(path), fontSize);
        if (!font.IsValid())
        {
            return nullptr;
        }

        auto [insertedIt, inserted] = m_Fonts.emplace(key, std::move(font));
        if (!inserted)
        {
            return nullptr;
        }

        return &insertedIt->second;
    }

    Font2D* FontManager::Get(const std::string& key)
    {
        auto it = m_Fonts.find(key);
        if (it == m_Fonts.end())
        {
            return nullptr;
        }

        return &it->second;
    }

    const Font2D* FontManager::Get(const std::string& key) const
    {
        auto it = m_Fonts.find(key);
        if (it == m_Fonts.end())
        {
            return nullptr;
        }

        return &it->second;
    }

    bool FontManager::Has(const std::string& key) const
    {
        return m_Fonts.find(key) != m_Fonts.end();
    }

    void FontManager::Unload(const std::string& key)
    {
        auto it = m_Fonts.find(key);
        if (it == m_Fonts.end())
        {
            return;
        }

        m_Graphics.DestroyFont(it->second);
        m_Fonts.erase(it);
    }

    void FontManager::Clear()
    {
        for (auto& [key, font] : m_Fonts)
        {
            m_Graphics.DestroyFont(font);
        }

        m_Fonts.clear();
    }
}