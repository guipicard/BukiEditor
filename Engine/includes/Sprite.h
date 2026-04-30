#pragma once

#include "Component.h"
#include "IDrawable.h"
#include "Graphics/Texture2D.h"
#include "BukiContainers.h"

#include <string>

namespace buki
{
    struct Sprite final : public Component, public IDrawable
    {
    public:
        explicit Sprite(Entity* entity);
        ~Sprite() override = default;

        void Draw(float alpha) override;

        json Serialize() override;
        void Deserialize(json doc) override;
        void Set() override;

        void SetPath(const std::string& path);
        const std::string& GetPath() const { return m_Path; }

        void SetColor(const Color& color) { m_Color = color; }
        const Color& GetColor() const { return m_Color; }

        void SetFlip(bool h, bool v) { m_Flip.h = h; m_Flip.v = v; }

    private:
        Texture2D* m_Texture = nullptr;
        std::string m_Path;

        Color m_Color{ 1.f, 1.f, 1.f, 1.f };
        Vector2 m_PositionOffset{ 0.f, 0.f };
        Vector2 m_SizeOffset{ 0.f, 0.f };

        Flip m_Flip{};
        RectF m_SourceRectPixels{};
        bool m_UseSourceRect = false;
    };
}