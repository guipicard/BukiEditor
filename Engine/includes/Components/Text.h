#pragma once

#include "Component.h"
#include "PropertyInfo.h"
#include "IDrawable.h"
#include "Graphics/Font2D.h"
#include "BukiContainers.h"

#include <string>

namespace buki
{
    class Text final : public Component, public IDrawable
    {
    public:
        explicit Text(Entity* entity);
        ~Text() override = default;

        void Draw(float alpha) override;
        void Set() override;

        void SetText(const std::string& text) { m_Text = text; }
        const std::string& GetText() const { return m_Text; }

		Color GetColor() const { return m_Color; }
		Color* GetColorRef() { return &m_Color; }
		bool GetCenterX() const { return m_CenterX; }
		bool GetCenterY() const { return m_CenterY; }
        bool* GetCenterXRef() { return &m_CenterX; }
        bool* GetCenterYRef() { return &m_CenterY; }
		int GetFontSize() const { return m_FontSize; }
		int* GetFontSizeRef() { return &m_FontSize; }
		Vector2 GetPositionOffset() const { return m_PositionOffset; }
		Vector2* GetPositionOffsetRef() { return &m_PositionOffset; }
		std::string GetFontPath() const { return m_FontPath; }
        void SetFontPath(const std::string& path) { m_FontPath = path; }
        void SetFontSize(int size) { m_FontSize = size; }
        void SetCentering(bool x, bool y) { m_CenterX = x; m_CenterY = y; }
		void SetColor(const Color& color) { m_Color = color; }
		void SetPositionOffset(const Vector2& offset) { m_PositionOffset = offset; }

        [[nodiscard]] Vector2 GetSize() const;

    public:
        const std::vector<PropertyInfo>& GetProperties() const override
        {
            static std::vector<PropertyInfo> properties = {
                BUKI_PROP_STRING_N("text", Text, m_Text),
                BUKI_PROP_STRING_N("fontPath", Text, m_FontPath),
                BUKI_PROP_INT_N("fontSize", Text, m_FontSize),
                BUKI_PROP_VECTOR2_N("positionOffset", Text, m_PositionOffset),
                BUKI_PROP_COLOR_N("color", Text, m_Color),
                BUKI_PROP_BOOL_N("centerX", Text, m_CenterX),
                BUKI_PROP_BOOL_N("centerY", Text, m_CenterY),
            };
            return properties;
        }
    private:
        std::string m_Text;
        std::string m_FontPath;

        int m_FontSize = 24;
        Color m_Color{ 1.f, 1.f, 1.f, 1.f };

        Vector2 m_PositionOffset{ 0.f, 0.f };
        bool m_CenterX = true;
        bool m_CenterY = true;
        Font2D* m_Font = nullptr;

    };
}