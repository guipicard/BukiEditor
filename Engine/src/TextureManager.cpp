#include "Graphics/TextureManager.h"

#include "Graphics/IGraphics.h"

namespace buki
{
    TextureManager::TextureManager(IGraphics& graphics)
        : m_Graphics(graphics)
    {
    }

    TextureManager::~TextureManager()
    {
        Clear();
    }

    Texture2D* TextureManager::Load(const std::string& path, bool flipVertically)
    {
        auto it = m_Textures.find(path);
        if (it != m_Textures.end())
        {
            return &it->second;
        }

        Texture2D texture = TextureLoader::LoadFromFile(m_Graphics, path, flipVertically);
        if (!texture.IsValid())
        {
            return nullptr;
        }

        auto [insertedIt, inserted] = m_Textures.emplace(path, std::move(texture));
        if (!inserted)
        {
            return nullptr;
        }

        return &insertedIt->second;
    }

    Texture2D* TextureManager::Get(const std::string& path)
    {
        auto it = m_Textures.find(path);
        if (it == m_Textures.end())
        {
            return nullptr;
        }

        return &it->second;
    }

    const Texture2D* TextureManager::Get(const std::string& path) const
    {
        auto it = m_Textures.find(path);
        if (it == m_Textures.end())
        {
            return nullptr;
        }

        return &it->second;
    }

    bool TextureManager::Has(const std::string& path) const
    {
        return m_Textures.find(path) != m_Textures.end();
    }

    void TextureManager::Unload(const std::string& path)
    {
        auto it = m_Textures.find(path);
        if (it == m_Textures.end())
        {
            return;
        }

        TextureLoader::Destroy(m_Graphics, it->second);
        m_Textures.erase(it);
    }

    void TextureManager::Clear()
    {
        for (auto& [path, texture] : m_Textures)
        {
            TextureLoader::Destroy(m_Graphics, texture);
        }

        m_Textures.clear();
    }
}
