#pragma once
#include "Text.h"

#include "Engine.h"
#include "Entity.h"

namespace buki
{
    Text::Text(Entity* entity)
        : Component(entity)
    {
    }

    void Text::Draw(float alpha)
    {
        if (m_Font == nullptr || !m_Font->IsValid() || m_Entity == nullptr)
        {
            return;
        }

        const Vector2 pos = m_Entity->Tm().GetPosition() + m_PositionOffset;

        Graphics().DrawTextToCamera(
            *m_Font,
            m_Text,
            Camera(),
            glm::vec2{ pos.x, pos.y },
            m_Color,
            m_CenterX,
            m_CenterY
        );

    }

    void Text::Set()
    {
        m_Font = nullptr;

        if (!m_FontPath.empty() && m_FontSize > 0)
        {
            m_Font = Fonts().Load(m_FontPath, m_FontSize);
        }
    }

    Vector2 buki::Text::GetSize() const
    {
        if (m_Font == nullptr || !m_Font->IsValid())
        {
            return Vector2{ 0.0f, 0.0f };
        }

        return Graphics().MeasureText(*m_Font, m_Text);
    }
}