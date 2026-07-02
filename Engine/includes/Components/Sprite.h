#pragma once

#include "Component.h"
#include "IDrawable.h"
#include "Graphics/Texture2D.h"
#include "BukiContainers.h"

#include <string>

namespace buki
{
	class Sprite final : public Component, public IDrawable
	{
	public:
		explicit Sprite(Entity* entity);
		~Sprite() override = default;

		void Draw(float alpha) override;

		void Set() override;

		void SetPath(const std::string& path);
		const std::string& GetPath() const { return m_Path; }

		void SetColor(const Color& color) { m_Color = color; }
		const Color& GetColor() const { return m_Color; }

		void SetFlip(bool h, bool v) { m_Flip.h = h; m_Flip.v = v; }

		void SetPositionOffset(const Vector2& value) { m_PositionOffset = value; }
		Vector2 GetPositionOffset() const { return m_PositionOffset; }

		void SetImageSize(Vector2 value);
		Vector2 GetImageSize() const { return m_ImageSize; }

		bool GetFlipX() const { return m_Flip.h; }
		bool GetFlipY() const { return m_Flip.v; }
		void SetFlipX(bool value) { m_Flip.h = value; }
		void SetFlipY(bool value) { m_Flip.v = value; }

		bool UsesSourceRect() const { return m_UseSourceRect; }
		void SetUseSourceRect(bool value) { m_UseSourceRect = value; }

		RectF GetSourceRectPixels() const { return m_SourceRectPixels; }
		void SetSourceRectPixels(const RectF& rect) { m_SourceRectPixels = rect; }

	public:
		const std::vector<PropertyInfo>& GetProperties() const override
		{
			static std::vector<PropertyInfo> properties = {
				BUKI_PROP_IMAGE_N("path", Sprite, m_Path),
				BUKI_PROP_VECTOR2_N("positionOffset", Sprite, m_PositionOffset),
				BUKI_PROP_VECTOR2_N("size", Sprite, m_ImageSize),
				BUKI_PROP_COLOR_N("color", Sprite, m_Color),
				BUKI_PROP_BOOL_N("useSourceRect", Sprite, m_UseSourceRect),
				BUKI_PROP_RECTF_N("sourceRectPixels", Sprite, m_SourceRectPixels),
				BUKI_PROP_BOOL_N("flipX", Sprite, m_Flip.h),
				BUKI_PROP_BOOL_N("flipY", Sprite, m_Flip.v),
			};
			return properties;
		}
	private:
		Texture2D* m_Texture = nullptr;
		Flip m_Flip{};
		std::string m_Path;
		Color m_Color{ 1.f, 1.f, 1.f, 1.f };
		Vector2 m_ImageSize;
		Vector2 m_PositionOffset{ 0.f, 0.f };
		RectF m_SourceRectPixels{};
		bool m_UseSourceRect = false;


	};
}