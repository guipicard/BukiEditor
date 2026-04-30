#pragma once

#include "Component.h"
#include "IDrawable.h"
#include "Graphics/Font2D.h"
#include "BukiContainers.h"

#include <string>

namespace buki
{
    struct Text final : public Component, public IDrawable
    {
    public:
        explicit Text(Entity* entity);
        ~Text() override = default;

        void Draw(float alpha) override;

        json Serialize() override;
        void Deserialize(json doc) override;
        void Set() override;

        void SetText(const std::string& text) { m_Text = text; }
        const std::string& GetText() const { return m_Text; }

        void SetFontPath(const std::string& path) { m_FontPath = path; }
        void SetFontSize(int size) { m_FontSize = size; }
        void SetCentering(bool x, bool y) { m_CenterX = x; m_CenterY = y; }
		void SetColor(const Color& color) { m_Color = color; }
		void SetPositionOffset(const Vector2& offset) { m_PositionOffset = offset; }

        [[nodiscard]] Vector2 GetSize() const;
    private:
        Font2D* m_Font = nullptr;

        std::string m_Text;
        std::string m_FontPath;

        int m_FontSize = 24;
        Color m_Color{ 1.f, 1.f, 1.f, 1.f };

        Vector2 m_PositionOffset{ 0.f, 0.f };
        bool m_CenterX = true;
        bool m_CenterY = true;
    };
}