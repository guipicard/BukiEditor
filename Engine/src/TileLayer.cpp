#include "TileLayer.h"

#include "Engine.h"
#include "Entity.h"

namespace buki
{
    TileLayer::TileLayer(Entity* entity)
        : Component(entity)
    {
    }

    void TileLayer::Draw(float alpha)
    {
        if (m_Atlas == nullptr || !m_Atlas->IsValid() || m_Entity == nullptr || m_Entity->T() == nullptr)
        {
            return;
        }

        Transform* t = m_Entity->T();
        const Vector2 entityPosition = t->GetPosition();
        const float rotation = t->GetRotation().GetRadians();

        for (const TileDrawData& tile : m_Tiles)
        {
            if (!tile.visible)
            {
                continue;
            }

            const Vector2 drawPos = entityPosition + m_LayerOffset + tile.localPosition;
            const Vector2 drawSize = (tile.size.x > 0.0f && tile.size.y > 0.0f) ? tile.size : m_DefaultTileSize;
            const Color finalColor = MultiplyColor(m_LayerTint, tile.color);

            Graphics().DrawSprite(
                *m_Atlas,
                Camera(),
                glm::vec2{ drawPos.x, drawPos.y },
                drawSize.x,
                drawSize.y,
                tile.sourceRectPixels,
                rotation,
                tile.flipX,
                tile.flipY,
                finalColor
            );
        }
    }

    json TileLayer::Serialize()
    {
        json doc;
        doc["atlasPath"] = m_AtlasPath;

        doc["defaultTileSize"]["x"] = m_DefaultTileSize.x;
        doc["defaultTileSize"]["y"] = m_DefaultTileSize.y;

        doc["layerOffset"]["x"] = m_LayerOffset.x;
        doc["layerOffset"]["y"] = m_LayerOffset.y;

        doc["layerTint"]["r"] = m_LayerTint.r;
        doc["layerTint"]["g"] = m_LayerTint.g;
        doc["layerTint"]["b"] = m_LayerTint.b;
        doc["layerTint"]["a"] = m_LayerTint.a;

        doc["defaultSrcRect"] = {
            {"x", m_DefaultSourceRectPixels.x},
            {"y", m_DefaultSourceRectPixels.y},
            {"w", m_DefaultSourceRectPixels.w},
            {"h", m_DefaultSourceRectPixels.h}
		};

		doc["defaultFlipX"] = m_DefaultFlipX;
		doc["defaultFlipY"] = m_DefaultFlipY;
		doc["defaultVisible"] = m_DefaultVisible;

        doc["tiles"] = json::array();

        for (const TileDrawData& tile : m_Tiles)
        {
            json t;
            t["localPosition"]["x"] = tile.localPosition.x;
            t["localPosition"]["y"] = tile.localPosition.y;

            t["size"]["x"] = tile.size.x;
            t["size"]["y"] = tile.size.y;

            t["sourceRectPixels"]["x"] = tile.sourceRectPixels.x;
            t["sourceRectPixels"]["y"] = tile.sourceRectPixels.y;
            t["sourceRectPixels"]["w"] = tile.sourceRectPixels.w;
            t["sourceRectPixels"]["h"] = tile.sourceRectPixels.h;

            t["color"]["r"] = tile.color.r;
            t["color"]["g"] = tile.color.g;
            t["color"]["b"] = tile.color.b;
            t["color"]["a"] = tile.color.a;

            t["flipX"] = tile.flipX;
            t["flipY"] = tile.flipY;
            t["visible"] = tile.visible;

            doc["tiles"].push_back(t);
        }

        return doc;
    }

    void TileLayer::Deserialize(json doc)
    {
        m_AtlasPath = doc.value("atlasPath", "");

        m_DefaultTileSize.x = doc["defaultTileSize"].value("x", 1.0f);
        m_DefaultTileSize.y = doc["defaultTileSize"].value("y", 1.0f);

        m_LayerOffset.x = doc["layerOffset"].value("x", 0.0f);
        m_LayerOffset.y = doc["layerOffset"].value("y", 0.0f);

        m_LayerTint.r = doc["layerTint"].value("r", 1.0f);
        m_LayerTint.g = doc["layerTint"].value("g", 1.0f);
        m_LayerTint.b = doc["layerTint"].value("b", 1.0f);
        m_LayerTint.a = doc["layerTint"].value("a", 1.0f);

        if (doc.contains("defaultSrcRect") && doc["defaultSrcRect"].is_object())
        {
            m_DefaultSourceRectPixels.x = doc["defaultSrcRect"].value("x", 0.0f);
            m_DefaultSourceRectPixels.y = doc["defaultSrcRect"].value("y", 0.0f);
            m_DefaultSourceRectPixels.w = doc["defaultSrcRect"].value("w", 1.0f);
            m_DefaultSourceRectPixels.h = doc["defaultSrcRect"].value("h", 1.0f);
        }
        else
        {
            m_DefaultSourceRectPixels = { 0.0f, 0.0f, 1.0f, 1.0f };
		}

        if (doc.contains("defaultFlipX"))
        {
            m_DefaultFlipX = doc.value("defaultFlipX", false);
        }
        else
        {
            m_DefaultFlipX = false;
		}
        if (doc.contains("defaultFlipY"))
        {
            m_DefaultFlipY = doc.value("defaultFlipY", false);
        }
        else
        {
            m_DefaultFlipY = false;
        }
        if (doc.contains("defaultVisible"))
        {
            m_DefaultVisible = doc.value("defaultVisible", true);
        }
        else
        {
            m_DefaultVisible = true;
		}

        m_Tiles.clear();

        if (doc.contains("tiles") && doc["tiles"].is_array())
        {
            for (const auto& t : doc["tiles"])
            {
                TileDrawData tile{};

                tile.localPosition.x = t["localPosition"].value("x", 0.0f);
                tile.localPosition.y = t["localPosition"].value("y", 0.0f);

                tile.size.x = t["size"].value("x", m_DefaultTileSize.x);
                tile.size.y = t["size"].value("y", m_DefaultTileSize.y);

                tile.sourceRectPixels.x = t["sourceRectPixels"].value("x", 0.0f);
                tile.sourceRectPixels.y = t["sourceRectPixels"].value("y", 0.0f);
                tile.sourceRectPixels.w = t["sourceRectPixels"].value("w", 0.0f);
                tile.sourceRectPixels.h = t["sourceRectPixels"].value("h", 0.0f);

                tile.color.r = t["color"].value("r", 1.0f);
                tile.color.g = t["color"].value("g", 1.0f);
                tile.color.b = t["color"].value("b", 1.0f);
                tile.color.a = t["color"].value("a", 1.0f);

                tile.flipX = t.value("flipX", false);
                tile.flipY = t.value("flipY", false);
                tile.visible = t.value("visible", true);

                m_Tiles.push_back(tile);
            }
        }
    }

    void TileLayer::Set()
    {
        m_Atlas = nullptr;

        if (!m_AtlasPath.empty())
        {
            m_Atlas = Textures().Load(m_AtlasPath);
        }
    }

    void TileLayer::SetAtlasPath(const std::string& path)
    {
        m_AtlasPath = path;
    }

    void TileLayer::ClearTiles()
    {
        m_Tiles.clear();
    }

    void TileLayer::AddTile(const TileDrawData& tile)
    {
        m_Tiles.push_back(tile);
    }

    void TileLayer::AddTile(const Vector2& localPosition, const Vector2& size, const RectF& sourceRectPixels)
    {
        TileDrawData tile{};
        tile.localPosition = localPosition;
        tile.size = size;
        tile.sourceRectPixels = sourceRectPixels;
        m_Tiles.push_back(tile);
    }

    void TileLayer::BuildUniformStrip(
        int count,
        const Vector2& startLocalPosition,
        const Vector2& step,
        const Vector2& tileSize,
        const RectF& sourceRectPixels)
    {
        if (count <= 0)
        {
            return;
        }

        for (int i = 0; i < count; ++i)
        {
            const Vector2 pos = {
                startLocalPosition.x + step.x * static_cast<float>(i),
                startLocalPosition.y + step.y * static_cast<float>(i)
            };

            AddTile(pos, tileSize, sourceRectPixels);
        }
    }

    void TileLayer::BuildGrid(
        int columns,
        int rows,
        const Vector2& originLocalPosition,
        const Vector2& step,
        const Vector2& tileSize,
        const RectF& sourceRectPixels)
    {
        if (columns <= 0 || rows <= 0)
        {
            return;
        }

        for (int y = 0; y < rows; ++y)
        {
            for (int x = 0; x < columns; ++x)
            {
                const Vector2 pos = {
                    originLocalPosition.x + step.x * static_cast<float>(x),
                    originLocalPosition.y + step.y * static_cast<float>(y)
                };

                AddTile(pos, tileSize, sourceRectPixels);
            }
        }
    }

    Color TileLayer::MultiplyColor(const Color& a, const Color& b)
    {
        return Color{
            a.r * b.r,
            a.g * b.g,
            a.b * b.b,
            a.a * b.a
        };
    }
}