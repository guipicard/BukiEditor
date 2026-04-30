#pragma once

#include "Graphics/IGraphics.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace buki
{
	struct Texture2D;

	class OpenGLGraphics final : public IGraphics
	{
	public:
		bool Initialize(IPlatform& platform) override;
		void Shutdown() override;

		void BeginFrame() override;
		void EndFrame() override;
		void Resize(int width, int height) override;

		void DrawQuad(
			const Camera2D& camera,
			const glm::vec2& worldPositionMeters,
			float widthMeters,
			float heightMeters,
			float rotationRadians,
			const UVRect& uvRect,
			const Color& color = {}) override;

		std::uint32_t CreateTextureRGBA8(const void* pixels, int width, int height) override;
		void DestroyTexture(std::uint32_t textureId) override;

		void DrawTexturedQuad(
			std::uint32_t textureId,
			const Camera2D& camera,
			const glm::vec2& worldPositionMeters,
			float widthMeters,
			float heightMeters,
			float rotationRadians,
			const UVRect& uvRect,
			const Color& color = {}) override;

		virtual void DrawLine(const Vector2& a, const Vector2& b, const Color& color) override;
		virtual void DrawRectOutline(const Vector2& center, const Vector2& size, float rotationRadians, const Color& color) override;
		virtual void FillRect(const Vector2& center, const Vector2& size, float rotationRadians, const Color& color) override;
		virtual void DrawCircleOutline(const Vector2& center, float radius, const Color& color, float rotationRadians = 0.0f) override;
		virtual void FillCircle(const Vector2& center, float radius, const Color& color) override;
		virtual void DrawPolygonOutline(const Vector2& center, float radius, float rotationRadians, const Color& color, int segments) override;
		virtual void FillPolygon(const Vector2& center, float radius, float rotationRadians, const Color& color, int segments) override;

		virtual std::vector<glm::vec2> GetPolygonPoints(glm::vec2 center, float radius, float rotationRadians, int segments, bool renderFill) override;

		void SetCameraPosition(float x, float y) override;
		void ResetCamera() override;

		void DrawSprite(
			const Texture2D& texture,
			const Camera2D& camera,
			const glm::vec2& worldPositionMeters,
			float widthMeters,
			float heightMeters,
			const RectF& sourceRectPixels,
			float rotationRadians,
			bool flipX = false,
			bool flipY = false,
			const Color& color = {}) override;

		void DrawSprite(
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
			const Color& color = {}) override;

		virtual void DrawSprite(
			const Texture2D& texture,
			const RectF& worldRectMeters,
			const RectF& sourceRectPixels,
			float rotationRadians,
			bool flipX = false,
			bool flipY = false) override;

		virtual void DrawSprite(
			const Texture2D& texture,
			const RectF& worldRectMeters,
			const RectF& sourceRectPixels,
			float rotationRadians,
			bool flipX = false,
			bool flipY = false,
			const Color& color = {}) override;

		Font2D CreateFontFromFile(const std::string& path, int fontSize) override;
		void DestroyFont(Font2D& font) override;

		virtual void DrawTextToCamera(
			const Font2D& font,
			const std::string& text,
			const Camera2D& camera,
			const glm::vec2& worldPositionMeters,
			const Color& color = {},
			bool centerX = false,
			bool centerY = false) override;

		virtual Vector2 MeasureText(const Font2D& font, const std::string& text) const override;

	private:

		bool InitializeLoader(IPlatform& platform);
		bool CreateShaderProgram();
		bool CreateQuadBuffers();

		void DestroyShaderProgram();
		void DestroyQuadBuffers();
		void UpdateProjection();


		UVRect BuildUVRect(
			const RectF& sourceRectPixels,
			float textureWidthPixels,
			float textureHeightPixels,
			bool flipX,
			bool flipY) const;

		void UploadQuadVertices(
			const glm::vec2& worldPositionMeters,
			float widthMeters,
			float heightMeters,
			float rotationRadians,
			const Color& color,
			const UVRect& uvRect,
			const Camera2D& camera) const;

		bool CompileShader(std::uint32_t shader, const char* source) const;
		bool LinkProgram(std::uint32_t program) const;

		private:
			GLuint m_DebugLineVAO = 0;
			GLuint m_DebugLineVBO = 0;
			GLuint m_DebugLineProgram = 0;
			GLint m_DebugLineProjLoc = -1;
			GLint m_DebugLineColorLoc = -1;

			bool CreateDebugLinePipeline();
			void DestroyDebugLinePipeline();
			void DrawPrimitiveInternal(const std::vector<glm::vec2>& points, const Color& color, GLenum mode);

			float MeasureTextWidthPixels(const Font2D& font, const std::string& text) const;

	private:
		IPlatform* m_Platform = nullptr;

		std::uint32_t m_ShaderProgram = 0;
		std::uint32_t m_VAO = 0;
		std::uint32_t m_VBO = 0;
		std::uint32_t m_EBO = 0;

		int m_ViewportWidth = 0;
		int m_ViewportHeight = 0;

		int m_ProjectionLocation = -1;
		glm::mat4 m_Projection{ 1.0f };

		float m_ClearColorR = 0.08f;
		float m_ClearColorG = 0.08f;
		float m_ClearColorB = 0.12f;
		float m_ClearColorA = 1.0f;

		std::uint32_t m_WhiteTexture = 0;
		int m_TextureLocation = -1;
		const unsigned char whitePixel[4] = { 255, 255, 255, 255 };

		glm::vec2 m_CameraPosition{ 0.0f, 0.0f };
	};
}