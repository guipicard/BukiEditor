#pragma once

#include "Graphics/IGraphics.h"
#include <cstdint>
#include <glm/glm.hpp>
#include "BukiContainers.h"

namespace buki
{
	struct Texture2D;
	class OpenGLGraphics final : public IGraphics
	{
	public:
		virtual bool Initialize(IPlatform& platform) override;
		virtual void Shutdown() override;

		virtual void BeginFrame() override;
		virtual void EndFrame() override;
		virtual void Resize(int width, int height) override;

		virtual void DrawQuad(float x, float y,
			float width, float height,
			float rotationRadians,
			float originX, float originY,
			float u0, float v0, float u1, float v1,
			float r, float g, float b, float a) override;

		virtual std::uint32_t CreateTextureRGBA8(const void* pixels, int width, int height) override;
		virtual void DestroyTexture(std::uint32_t textureId) override;
		virtual void DrawTexturedQuad(std::uint32_t textureId,
			float x, float y, float width, float height,
			float rotationRadians,
			float originX, float originY,
			float u0, float v0, float u1, float v1,
			float r, float g, float b, float a) override;

		void SetCameraPosition(float x, float y) override;
		void ResetCamera() override;

		virtual void DrawSprite(std::uint32_t textureId,
			float x, float y,
			float width, float height,
			const RectF& sourceRectPixels,
			float textureWidth, float textureHeight,
			float rotationRadians,
			float originX, float originY,
			float r, float g, float b, float a,
			bool flipX, bool flipY) override;

		virtual void DrawSprite(const Texture2D& texture,
			float x, float y,
			float width, float height,
			const RectF& sourceRectPixels,
			float rotationRadians,
			float originX, float originY,
			float r, float g, float b, float a,
			bool flipX, bool flipY) override;
	private:
		struct Vertex
		{
			float x, y;
			float r, g, b, a;
			float u, v;
		};

		bool InitializeLoader(IPlatform& platform);
		bool CreateShaderProgram();
		bool CreateQuadBuffers();

		void DestroyShaderProgram();
		void DestroyQuadBuffers();
		void UpdateProjection();
		void UploadQuadVertices(float x, float y, float width, float height,
			float rotationRadians,
			float originX, float originY,
			float u0, float v0, float u1, float v1,
			float r, float g, float b, float a);

		bool CompileShader(std::uint32_t shader, const char* source) const;
		bool LinkProgram(std::uint32_t program) const;

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
