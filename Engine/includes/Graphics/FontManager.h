#pragma once

#include "Graphics/Font2D.h"

#include <string>
#include <unordered_map>

namespace buki
{
    class IGraphics;

    class FontManager
    {
    public:
        explicit FontManager(IGraphics& graphics);
        ~FontManager();

        FontManager(const FontManager&) = delete;
        FontManager& operator=(const FontManager&) = delete;

        FontManager(FontManager&&) = delete;
        FontManager& operator=(FontManager&&) = delete;

    public:
        Font2D* Load(const std::string& path, int fontSize);
        Font2D* Get(const std::string& key);
        const Font2D* Get(const std::string& key) const;

        bool Has(const std::string& key) const;

        void Unload(const std::string& key);
        void Clear();

        [[nodiscard]] std::size_t Count() const
        {
            return m_Fonts.size();
        }

    private:
        std::string BuildAssetPath(const std::string& path) const;
        std::string BuildKey(const std::string& path, int fontSize) const;

    private:
        IGraphics& m_Graphics;
        std::unordered_map<std::string, Font2D> m_Fonts;
    };
}