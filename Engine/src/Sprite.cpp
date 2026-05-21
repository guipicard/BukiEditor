#include "Sprite.h"

#include "Engine.h"
#include "Entity.h"

namespace buki
{
    Sprite::Sprite(Entity* entity)
        : Component(entity)
    {
        if (m_Entity != nullptr && m_Entity->T() != nullptr)
        {
            m_SizeOffset = m_Entity->T()->GetSize();
        }
    }

    void Sprite::Draw(float alpha)
    {
        if (m_Texture == nullptr || !m_Texture->IsValid() || m_Entity == nullptr || m_Entity->T() == nullptr)
        {
            return;
        }

        Transform* t = m_Entity->T();
        const Vector2 pos = t->GetPosition() + m_PositionOffset;
        const Vector2 size = (m_SizeOffset.x != 0.0f && m_SizeOffset.y != 0.0f)
            ? m_SizeOffset
            : t->GetSize();

        RectF source = m_SourceRectPixels;
        if (!m_UseSourceRect)
        {
            source = RectF{
                0.0f,
                0.0f,
                static_cast<float>(m_Texture->width),
                static_cast<float>(m_Texture->height)
            };
        }

        Graphics().DrawSprite(
            *m_Texture,
            Camera(),
            glm::vec2{ pos.x, pos.y },
            size.x,
            size.y,
            source,
            t->GetRotation().GetRadians(),
            m_Flip.h,
            m_Flip.v,
            m_Color
        );
    }

    json Sprite::Serialize()
    {
        json doc;
        doc["path"] = m_Path;
        doc["positionOffset"]["x"] = m_PositionOffset.x;
        doc["positionOffset"]["y"] = m_PositionOffset.y;
        doc["size"]["x"] = m_SizeOffset.x;
        doc["size"]["y"] = m_SizeOffset.y;
        doc["color"]["r"] = m_Color.r;
        doc["color"]["g"] = m_Color.g;
        doc["color"]["b"] = m_Color.b;
        doc["color"]["a"] = m_Color.a;
        doc["flip"]["h"] = m_Flip.h;
        doc["flip"]["v"] = m_Flip.v;
        doc["useSourceRect"] = m_UseSourceRect;
        doc["sourceRect"]["x"] = m_SourceRectPixels.x;
        doc["sourceRect"]["y"] = m_SourceRectPixels.y;
        doc["sourceRect"]["w"] = m_SourceRectPixels.w;
        doc["sourceRect"]["h"] = m_SourceRectPixels.h;
        return doc;
    }

    void Sprite::Deserialize(json doc)
    {
        m_Path = doc.value("path", "");
        m_PositionOffset.x = doc["positionOffset"].value("x", 0.0f);
        m_PositionOffset.y = doc["positionOffset"].value("y", 0.0f);
        m_SizeOffset.x = doc["size"].value("x", 0.0f);
        m_SizeOffset.y = doc["size"].value("y", 0.0f);

        m_Color.r = doc["color"].value("r", 1.0f);
        m_Color.g = doc["color"].value("g", 1.0f);
        m_Color.b = doc["color"].value("b", 1.0f);
        m_Color.a = doc["color"].value("a", 1.0f);

        m_Flip.h = doc["flip"].value("h", false);
        m_Flip.v = doc["flip"].value("v", false);

        m_UseSourceRect = doc.value("useSourceRect", false);
        m_SourceRectPixels.x = doc["sourceRect"].value("x", 0.0f);
        m_SourceRectPixels.y = doc["sourceRect"].value("y", 0.0f);
        m_SourceRectPixels.w = doc["sourceRect"].value("w", 0.0f);
        m_SourceRectPixels.h = doc["sourceRect"].value("h", 0.0f);
    }

    void Sprite::Set()
    {
        m_Texture = nullptr;

        if (!m_Path.empty())
        {
            m_Texture = Textures().Load(m_Path, m_Path[1] != ':');
        }
    }

    void Sprite::SetPath(const std::string& path)
    {
        m_Path = path;
    }
}