#pragma once

#include <string>
#include <cstdint>

#include "Memory.h"
#include "BukiContainers.h"
#include "Graphics/Camera2D.h"
#include "glm/glm.hpp"

namespace buki
{
	class IPlatform;
	struct Texture2D;

	class IGraphics
	{
	public:
		virtual ~IGraphics() = default;

		virtual bool Initialize(IPlatform& platform) = 0;
		virtual void Shutdown() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void Resize(int width, int height) = 0;

		virtual void DrawQuad(
			const Camera2D& camera,
			const glm::vec2& worldPositionMeters,
			float widthMeters,
			float heightMeters,
			float rotationRadians,
			const UVRect& uvRect,
			const Color& color = {}) = 0;

		virtual std::uint32_t CreateTextureRGBA8(const void* pixels, int width, int height) = 0;
		virtual void DestroyTexture(std::uint32_t textureId) = 0;

		virtual void DrawTexturedQuad(
			std::uint32_t textureId,
			const Camera2D& camera,
			const glm::vec2& worldPositionMeters,
			float widthMeters,
			float heightMeters,
			float rotationRadians,
			const UVRect& uvRect,
			const Color& color = {}) = 0;

		virtual void DrawLine(const Vector2& a, const Vector2& b, const Color& color) = 0;
		virtual void DrawRectOutline(const Vector2& center, const Vector2& size, float rotationRadians, const Color& color) = 0;
		virtual void FillRect(const Vector2& center, const Vector2& size, float rotationRadians, const Color& color) = 0;
		virtual void DrawCircleOutline(const Vector2& center, float radius, const Color& color) = 0;
		virtual void FillCircle(const Vector2& center, float radius, const Color& color) = 0;
		virtual void DrawPolygonOutline(const Vector2& center, float radius, float rotationRadians, const Color& color, int segments) = 0;
		virtual void FillPolygon(const Vector2& center, float radius, float rotationRadians, const Color& color, int segments) = 0;

		virtual std::vector<glm::vec2> GetPolygonPoints(glm::vec2 center, float radius, float rotationRadians, int segments, bool renderFill) = 0;

		virtual void SetCameraPosition(float x, float y) = 0;
		virtual void ResetCamera() = 0;

		virtual void DrawSprite(
			const Texture2D& texture,
			const Camera2D& camera,
			const glm::vec2& worldPositionMeters,
			float widthMeters,
			float heightMeters,
			const RectF& sourceRectPixels,
			float rotationRadians,
			bool flipX = false,
			bool flipY = false,
			const Color& color = {}) = 0;

		virtual void DrawSprite(
			std::uint32_t textureId,
			float textureWidthPixels,
			float textureHeightPixels,
			const Camera2D& camera,
			const glm::vec2& worldPositionMeters,
			float widthMeters,
			float heightMeters,
			const RectF& sourceRectPixels,
			float rotationRadians,
			bool flipX = false,
			bool flipY = false,
			const Color& color = {}) = 0;

		virtual void DrawSprite(
			const Texture2D& texture,
			const RectF& worldRectMeters,
			const RectF& sourceRectPixels,
			float rotationRadians,
			bool flipX = false,
			bool flipY = false) = 0;

		virtual void DrawSprite(
			const Texture2D& texture,
			const RectF& worldRectMeters,
			const RectF& sourceRectPixels,
			float rotationRadians,
			bool flipX = false,
			bool flipY = false,
			const Color& color = {}) = 0;
	};
}
