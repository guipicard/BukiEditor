#pragma once

#include "Component.h"
#include "IDrawable.h"
#include "Graphics/Texture2D.h"
#include "BukiContainers.h"
#include "PropertyInfo.h"

#include <string>
#include <vector>

namespace buki
{
	struct TileDrawData
	{
		std::string texturePath;
		Vector2 localPosition{ 0.0f, 0.0f };
		Vector2 size{ 1.0f, 1.0f };
		RectF sourceRectPixels;

		Color color{ 1.0f, 1.0f, 1.0f, 1.0f };

		bool flipX = false;
		bool flipY = false;
	};

	class TileLayer final : public Component, public IDrawable
	{
	public:
		explicit TileLayer(Entity* entity);
		~TileLayer() override = default;

		void Draw(float alpha) override;

		virtual void Set() override;

		void ClearTiles();
		void AddTile(const TileDrawData& tile);
		void AddTile(const Vector2& localPosition, const Vector2& size, const RectF& sourceRectPixels);

		std::vector<TileDrawData>& GetTiles() { return m_Tiles; }
		const std::vector<TileDrawData>& GetTiles() const { return m_Tiles; }

		void SetDefaultTileSize(const Vector2& size) { m_DefaultTileSize = size; }
		Vector2 GetDefaultTileSize() const { return m_DefaultTileSize; }

		void SetDefaultSourceRectPixels(const RectF& rect) { m_DefaultSourceRectPixels = rect; }
		RectF GetDefaultSourceRectPixels() const { return m_DefaultSourceRectPixels; }

		void SetLayerOffset(const Vector2& offset) { m_LayerOffset = offset; }
		Vector2 GetLayerOffset() const { return m_LayerOffset; }

		void SetDefaultFlipX(bool flipX) { m_DefaultFlipX = flipX; }
		bool GetDefaultFlipX() const { return m_DefaultFlipX; }

		void SetDefaultFlipY(bool flipY) { m_DefaultFlipY = flipY; }
		bool GetDefaultFlipY() const { return m_DefaultFlipY; }

		void SetTint(const Color& tint) { m_LayerTint = tint; }
		const Color& GetTint() const { return m_LayerTint; }

		void BuildUniformStrip(
			int count,
			const Vector2& startLocalPosition,
			const Vector2& step,
			const Vector2& tileSize,
			const RectF& sourceRectPixels);

		void BuildGrid(
			int columns,
			int rows,
			const Vector2& originLocalPosition,
			const Vector2& step,
			const Vector2& tileSize,
			const RectF& sourceRectPixels);

		Texture2D* GetTexture() const { return m_Atlas; }

		const std::string GetTexturePath() { return m_AtlasPath; }
		void SetTexturePath(const std::string& path) { m_AtlasPath = path; }

	public:
		const std::vector<PropertyInfo>& GetProperties() const override
		{
			static std::vector<PropertyInfo> properties = {
				BUKI_PROP_IMAGE_GROUPED("atlasPath", TileLayer, m_AtlasPath, "Rendering"),
				BUKI_PROP_VECTOR2_GROUPED("defaultTileSize", TileLayer, m_DefaultTileSize, "Rendering"),
				BUKI_PROP_RECTF_GROUPED("defaultSrcRect", TileLayer, m_DefaultSourceRectPixels, "Rendering"),
				BUKI_PROP_COLOR_GROUPED("layerTint", TileLayer, m_LayerTint, "Rendering"),
				BUKI_PROP_VECTOR2_GROUPED("layerOffset", TileLayer, m_LayerOffset, "Rendering"),
				BUKI_PROP_BOOL_GROUPED("defaultFlipX", TileLayer, m_DefaultFlipX, "Rendering"),
				BUKI_PROP_BOOL_GROUPED("defaultFlipY", TileLayer, m_DefaultFlipY, "Rendering"),
				// TODO: Add tile list editor
				BUKI_PROP_TILE_LIST_GROUPED("tiles", TileLayer, m_Tiles, "Rendering"),
			};
			return properties;
		}


	private:
		static Color MultiplyColor(const Color& a, const Color& b);
	private:
		Texture2D* m_Atlas = nullptr;
		std::string m_AtlasPath;
		Vector2 m_DefaultTileSize{ 1.0f, 1.0f };
		RectF m_DefaultSourceRectPixels{ 0.0f, 0.0f, 1.0f, 1.0f };
		Vector2 m_LayerOffset{ 0.0f, 0.0f };
		std::vector<TileDrawData> m_Tiles;
		Color m_LayerTint{ 1.0f, 1.0f, 1.0f, 1.0f };
		bool m_DefaultFlipX = false;
		bool m_DefaultFlipY = false;
	};
}