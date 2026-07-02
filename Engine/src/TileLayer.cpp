#pragma once
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
        if (m_Atlas == nullptr || !m_Atlas->IsValid() || m_Entity == nullptr)
        {
            return;
        }

        const auto& t = m_Entity->Tm();
        const Vector2 entityPosition = t.GetPosition();
        const float rotation = t.GetRotation().GetRadians();

        for (const TileDrawData& tile : m_Tiles)
        {

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

    void TileLayer::Set()
    {
        m_Atlas = nullptr;

        if (!m_AtlasPath.empty())
        {
            m_Atlas = Textures().Load(m_AtlasPath);
        }
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