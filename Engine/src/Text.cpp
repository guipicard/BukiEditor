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
        if (m_Font == nullptr || !m_Font->IsValid() || m_Entity == nullptr || m_Entity->T() == nullptr)
        {
            return;
        }

        const Vector2 pos = m_Entity->T()->GetPosition() + m_PositionOffset;

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

    json Text::Serialize()
    {
        json doc;
        doc["text"] = m_Text;
        doc["fontPath"] = m_FontPath;
        doc["fontSize"] = m_FontSize;
        doc["positionOffset"]["x"] = m_PositionOffset.x;
        doc["positionOffset"]["y"] = m_PositionOffset.y;
        doc["color"]["r"] = m_Color.r;
        doc["color"]["g"] = m_Color.g;
        doc["color"]["b"] = m_Color.b;
        doc["color"]["a"] = m_Color.a;
        doc["centerX"] = m_CenterX;
        doc["centerY"] = m_CenterY;
        return doc;
    }

    void Text::Deserialize(json doc)
    {
        m_Text = doc.value("text", "");
        m_FontPath = doc.value("fontPath", "");
        m_FontSize = doc.value("fontSize", 24);

        m_PositionOffset.x = doc["positionOffset"].value("x", 0.0f);
        m_PositionOffset.y = doc["positionOffset"].value("y", 0.0f);

        m_Color.r = doc["color"].value("r", 1.0f);
        m_Color.g = doc["color"].value("g", 1.0f);
        m_Color.b = doc["color"].value("b", 1.0f);
        m_Color.a = doc["color"].value("a", 1.0f);

        m_CenterX = doc.value("centerX", true);
        m_CenterY = doc.value("centerY", true);
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