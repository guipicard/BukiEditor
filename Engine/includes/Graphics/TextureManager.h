#pragma once

#include "Graphics/Texture2D.h"

#include <string>
#include <unordered_map>

namespace buki
{
    class IGraphics;

    class TextureManager
    {
    public:
        explicit TextureManager(IGraphics& graphics);
        ~TextureManager();

        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;

        TextureManager(TextureManager&&) = delete;
        TextureManager& operator=(TextureManager&&) = delete;

    public:
        Texture2D* Load(const std::string& path);
        Texture2D* Get(const std::string& path);
        const Texture2D* Get(const std::string& path) const;

        bool Has(const std::string& path) const;

        void Unload(const std::string& path);
        void Clear();

        [[nodiscard]] std::size_t Count() const
        {
            return m_Textures.size();
        }

    private:
        IGraphics& m_Graphics;
        std::unordered_map<std::string, Texture2D> m_Textures;
    };
}
