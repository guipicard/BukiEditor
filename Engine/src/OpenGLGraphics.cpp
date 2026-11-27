#pragma once
#define NOMINMAX
#include <Windows.h>

#include "Graphics/OpenGLGraphics.h"
#include "Platform/IPlatform.h"
#include "Graphics/Texture2D.h"
#include "Camera2DUtils.h"
#include "Units.h"
#include "Engine.h"
#include "PolygonGeometry.h"

#include "Memory.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <string>

#include "Graphics/Font2D.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>

//#define BUKI_DEBUG_TEXT_LAYOUT

using TtfFontPtr = std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>;
using SurfacePtr = std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)>;

namespace
{
	buki::IPlatform* g_OpenGLPlatform = nullptr;

	void* LoadOpenGLProc(const char* procName)
	{
		return g_OpenGLPlatform != nullptr
			? g_OpenGLPlatform->GetProcAddress(procName)
			: nullptr;
	}

	constexpr const char* kDebugLineVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 uProjection;

void main()
{
    gl_Position = uProjection * vec4(aPos.xy, 0.0, 1.0);
}
)";

	constexpr const char* kDebugLineFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

uniform vec4 uColor;

void main()
{
    FragColor = uColor;
}
)";
	constexpr const char* kVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aUV;

uniform mat4 uProjection;

out vec4 vColor;
out vec2 vUV;

void main()
{
	gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
	vColor = aColor;
	vUV = aUV;
}
)";

	constexpr const char* kFragmentShaderSource = R"(
#version 330 core
in vec4 vColor;
in vec2 vUV;

uniform sampler2D uTexture;

out vec4 FragColor;

void main()
{
	FragColor = texture(uTexture, vUV) * vColor;
}
)";

	constexpr unsigned int kQuadIndices[6] =
	{
		0, 1, 2,
		2, 3, 0
	};

	glm::mat4 GetCurrentViewportProjection()
	{
		GLint viewport[4] = {};
		glGetIntegerv(GL_VIEWPORT, viewport);

		const float width =
			static_cast<float>(std::max(1, viewport[2]));

		const float height =
			static_cast<float>(std::max(1, viewport[3]));

		return glm::ortho(
			0.0f,
			width,
			height,
			0.0f,
			-1.0f,
			1.0f);
	}
}

bool buki::OpenGLGraphics::Initialize(IPlatform& platform)
{
	m_Platform = &platform;

	if (!InitializeLoader(platform))
	{
		Shutdown();
		return false;
	}

	m_ViewportWidth = platform.GetDrawableWidth();
	m_ViewportHeight = platform.GetDrawableHeight();

	glViewport(0, 0, m_ViewportWidth, m_ViewportHeight);
	glClearColor(m_ClearColorR, m_ClearColorG, m_ClearColorB, m_ClearColorA);

	if (!CreateShaderProgram())
	{
		OutputDebugStringA("CreateShaderProgram failed.\n");
		Shutdown();
		return false;
	}

	if (!CreateQuadBuffers())
	{
		Shutdown();
		return false;
	}
	if (!CreateDebugLinePipeline())
	{
		Shutdown();
		return false;
	}
	UpdateProjection();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_WhiteTexture = CreateTextureRGBA8(whitePixel, 1, 1);
	if (m_WhiteTexture == 0)
	{
		Shutdown();
		return false;
	}

	return true;
}

void buki::OpenGLGraphics::Shutdown()
{
	DestroyQuadBuffers();
	DestroyDebugLinePipeline();
	DestroyShaderProgram();

	DestroyTexture(m_WhiteTexture);
	m_WhiteTexture = 0;

	m_ProjectionLocation = -1;
	m_TextureLocation = -1;
	m_ViewportWidth = 0;
	m_ViewportHeight = 0;
	m_Platform = nullptr;
	g_OpenGLPlatform = nullptr;

	TTF_Quit();
}

void buki::OpenGLGraphics::BeginFrame()
{
	glViewport(0, 0, m_ViewportWidth, m_ViewportHeight);
	glClearColor(m_ClearColorR, m_ClearColorG, m_ClearColorB, m_ClearColorA);
	glClear(GL_COLOR_BUFFER_BIT);
}

void buki::OpenGLGraphics::EndFrame()
{
	if (m_Platform != nullptr)
	{
		m_Platform->SwapBuffers();
	}
}

void buki::OpenGLGraphics::Resize(int width, int height)
{
	m_ViewportWidth = width;
	m_ViewportHeight = height;
	glViewport(0, 0, m_ViewportWidth, m_ViewportHeight);
	UpdateProjection();
}

void buki::OpenGLGraphics::DrawQuad(
	const Camera2D& camera,
	const glm::vec2& worldPositionMeters,
	float widthMeters,
	float heightMeters,
	float rotationRadians,
	const UVRect& uvRect,
	const Color& color)
{
	DrawTexturedQuad(
		m_WhiteTexture,
		camera,
		worldPositionMeters,
		widthMeters,
		heightMeters,
		rotationRadians,
		uvRect,
		color
	);
}

std::uint32_t buki::OpenGLGraphics::CreateTextureRGBA8(const void* pixels, int width, int height)
{
	if (pixels == nullptr || width <= 0 || height <= 0)
	{
		return 0;
	}

	while (glGetError() != GL_NO_ERROR) {}

	std::uint32_t textureId = 0;
	glGenTextures(1, &textureId);
	if (textureId == 0)
	{
		OutputDebugStringA("glGenTextures failed\n");
		return 0;
	}

	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA8,
		width,
		height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		pixels);

	const GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		char buffer[128];

		sprintf_s(buffer, "glTexImage2D failed, error = 0x%X\n", err);
		OutputDebugStringA(buffer);

		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &textureId);
		return 0;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	return textureId;
}

void buki::OpenGLGraphics::DestroyTexture(std::uint32_t textureId)
{
	if (textureId != 0)
	{
		glDeleteTextures(1, &textureId);
	}
}

void buki::OpenGLGraphics::DrawTexturedQuad(
	std::uint32_t textureId,
	const Camera2D& camera,
	const glm::vec2& worldPositionMeters,
	float widthMeters,
	float heightMeters,
	float rotationRadians,
	const UVRect& uvRect,
	const Color& color)
{
	if (textureId == 0 || m_ShaderProgram == 0 || m_VAO == 0 || m_VBO == 0 || m_EBO == 0)
	{
		return;
	}

	UploadQuadVertices(
		worldPositionMeters,
		widthMeters,
		heightMeters,
		rotationRadians,
		color,
		uvRect,
		camera
	);

	const glm::mat4 projection =
		GetCurrentViewportProjection();

	glUseProgram(m_ShaderProgram);

	glUniformMatrix4fv(
		m_ProjectionLocation,
		1,
		GL_FALSE,
		glm::value_ptr(projection));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	if (m_TextureLocation >= 0)
	{
		glUniform1i(m_TextureLocation, 0);
	}

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void buki::OpenGLGraphics::DrawLine(
	const Vector2& a,
	const Vector2& b,
	const Color& color,
	const LineStyle& lineStyle)
{
	const Camera2D& camera = Engine::Get().GetActiveCamera();
	const float thicknessPixels = GetLineThicknessPixels(lineStyle, camera);

	const std::vector<glm::vec2> points =
	{
		WorldToScreen(glm::vec2{ a.x, a.y }, camera),
		WorldToScreen(glm::vec2{ b.x, b.y }, camera)
	};

	DrawStrokeInternal(points, false, color, thicknessPixels);
}

void buki::OpenGLGraphics::DrawRectOutline(
	const Vector2& center,
	const Vector2& size,
	float rotationRadians,
	const Color& color,
	const LineStyle& lineStyle)
{
	if (size.x <= 0.0f || size.y <= 0.0f)
	{
		return;
	}

	const Camera2D& camera = Engine::Get().GetActiveCamera();
	const glm::vec2 screenCenter = WorldToScreen(glm::vec2{ center.x, center.y }, camera);
	const float thicknessPixels = GetLineThicknessPixels(lineStyle, camera);

	const glm::vec2 halfSize =
	{
		WorldToScreenSize(size.x, camera) * 0.5f,
		WorldToScreenSize(size.y, camera) * 0.5f
	};

	const float screenRotation = WorldToScreenRotation(rotationRadians);
	const float cosine = std::cos(screenRotation);
	const float sine = std::sin(screenRotation);

	const auto rotate = [cosine, sine](const glm::vec2& point) -> glm::vec2
		{
			return
			{
				point.x * cosine - point.y * sine,
				point.x * sine + point.y * cosine
			};
		};

	const std::vector<glm::vec2> points =
	{
		screenCenter + rotate({ -halfSize.x, -halfSize.y }),
		screenCenter + rotate({ halfSize.x, -halfSize.y }),
		screenCenter + rotate({ halfSize.x, halfSize.y }),
		screenCenter + rotate({ -halfSize.x, halfSize.y })
	};

	DrawStrokeInternal(points, true, color, thicknessPixels);
}

void buki::OpenGLGraphics::DrawCircleOutline(
	const Vector2& center,
	float radius,
	const Color& color,
	float rotationRadians,
	const LineStyle& lineStyle)
{
	if (radius <= 0.0f)
	{
		return;
	}

	const Camera2D& camera = Engine::Get().GetActiveCamera();
	const glm::vec2 screenCenter = WorldToScreen(glm::vec2{ center.x, center.y }, camera);
	const float screenRadius = std::abs(WorldToScreenSize(radius, camera));
	const float thicknessPixels = GetLineThicknessPixels(lineStyle, camera);

	const int segments = static_cast<int>(std::clamp(screenRadius, 16.0f, 128.0f));
	const std::vector<glm::vec2> points = GetPolygonPoints(screenCenter, screenRadius, rotationRadians, segments, false);

	DrawStrokeInternal(points, true, color, thicknessPixels);

	// Preserve the existing rotation-indicator radius.
	const std::vector<glm::vec2> radiusPoints =
	{
		screenCenter,
		points.front()
	};

	DrawStrokeInternal(radiusPoints, false, color, thicknessPixels);
}

void buki::OpenGLGraphics::DrawPolygonOutline(
	const Vector2& center,
	float radius,
	float rotationRadians,
	const Color& color,
	int segments,
	const LineStyle& lineStyle)
{
	if (radius <= 0.0f)
	{
		return;
	}

	segments = std::max(segments, 3);

	const Camera2D& camera = Engine::Get().GetActiveCamera();
	const glm::vec2 screenCenter = WorldToScreen(glm::vec2{ center.x, center.y }, camera);
	const float screenRadius = std::abs(WorldToScreenSize(radius, camera));
	const float thicknessPixels = GetLineThicknessPixels(lineStyle, camera);

	const std::vector<glm::vec2> points = GetPolygonPoints(screenCenter, screenRadius, rotationRadians, segments, false);

	DrawStrokeInternal(points, true, color, thicknessPixels);
}

void buki::OpenGLGraphics::FillRect(
	const Vector2& center,
	const Vector2& size,
	float rotationRadians,
	const Color& color)
{
	if (size.x <= 0.0f || size.y <= 0.0f)
	{
		return;
	}

	const Camera2D& camera = Engine::Get().GetActiveCamera();
	const glm::vec2 screenCenter = WorldToScreen(glm::vec2{ center.x, center.y }, camera);

	const glm::vec2 halfSize =
	{
		WorldToScreenSize(size.x, camera) * 0.5f,
		WorldToScreenSize(size.y, camera) * 0.5f
	};

	const float screenRotation = WorldToScreenRotation(rotationRadians);
	const float cosine = std::cos(screenRotation);
	const float sine = std::sin(screenRotation);

	const auto rotate = [cosine, sine](const glm::vec2& point) -> glm::vec2
		{
			return
			{
				point.x * cosine - point.y * sine,
				point.x * sine + point.y * cosine
			};
		};

	const std::vector<glm::vec2> points =
	{
		screenCenter,
		screenCenter + rotate({ -halfSize.x, -halfSize.y }),
		screenCenter + rotate({ halfSize.x, -halfSize.y }),
		screenCenter + rotate({ halfSize.x, halfSize.y }),
		screenCenter + rotate({ -halfSize.x, halfSize.y }),
		screenCenter + rotate({ -halfSize.x, -halfSize.y })
	};

	DrawPrimitiveInternal(points, color, GL_TRIANGLE_FAN);
}

void buki::OpenGLGraphics::FillCircle(const Vector2& center, float radius, const Color& color)
{
	const Camera2D& camera = Engine::Get().GetActiveCamera();
	const glm::vec2 screenCenter = WorldToScreen(glm::vec2{ center.x, center.y }, camera);
	const float screenRadius = WorldToScreenSize(radius, camera);
	int segments = std::clamp(static_cast<int>(screenRadius), 16, 128);

	std::vector<glm::vec2> points;
	points.reserve(static_cast<size_t>(segments + 2));

	points.push_back(screenCenter);

	const float step = 6.28318530718f / static_cast<float>(segments);
	const float angleOffset = 6.28318530718f / 4;
	for (int i = 0; i <= segments; ++i)
	{
		const float a = (static_cast<float>(i) * step) - angleOffset;
		points.push_back(
			{
				screenCenter.x + std::cos(a) * screenRadius,
				screenCenter.y + std::sin(a) * screenRadius
			});
	}

	DrawPrimitiveInternal(points, color, GL_TRIANGLE_FAN);
}

void buki::OpenGLGraphics::FillPolygon(const Vector2& center, float radius, float rotationRadians, const Color& color, int segments)
{
	if (segments < 3)
	{
		segments = 3;
	}
	const Camera2D& camera = Engine::Get().GetActiveCamera();
	const glm::vec2 screenCenter = WorldToScreen(glm::vec2{ center.x, center.y }, camera);
	const float screenRadius = WorldToScreenSize(radius, camera);
	const std::vector<glm::vec2>& points = GetPolygonPoints(screenCenter, screenRadius, rotationRadians, segments, true);
	DrawPrimitiveInternal(points, color, GL_TRIANGLE_FAN);
}

std::vector<glm::vec2> buki::OpenGLGraphics::GetPolygonPoints(
	glm::vec2 center,
	float radius,
	float rotationRadians,
	int segments,
	bool renderFill)
{
	const std::vector<glm::vec2> localVertices = MakeRegularPolygonVertices(radius, segments, rotationRadians);

	std::vector<glm::vec2> points;
	points.reserve(localVertices.size() + (renderFill ? 2 : 0));

	if (renderFill)
	{
		points.push_back(center);
	}

	for (const glm::vec2& vertex : localVertices)
	{
		// This existing graphics API receives a screen-space center
		// and radius, so invert the local world Y offset here.
		points.push_back(
			{
				center.x + vertex.x,
				center.y - vertex.y
			});
	}

	if (renderFill && !localVertices.empty())
	{
		points.push_back(points[1]);
	}

	return points;
}


void buki::OpenGLGraphics::SetCameraPosition(float x, float y)
{
	m_CameraPosition = { x, y };
}

void buki::OpenGLGraphics::ResetCamera()
{
	m_CameraPosition = { 0.0f, 0.0f };
}

void buki::OpenGLGraphics::DrawSprite(
	const Texture2D& texture,
	const Camera2D& camera,
	const glm::vec2& worldPositionMeters,
	float widthMeters,
	float heightMeters,
	const RectF& sourceRectPixels,
	float rotationRadians,
	bool flipX,
	bool flipY,
	const Color& color)
{
	DrawSprite(
		texture.id,
		static_cast<float>(texture.width),
		static_cast<float>(texture.height),
		camera,
		worldPositionMeters,
		widthMeters,
		heightMeters,
		sourceRectPixels,
		rotationRadians,
		flipX,
		flipY,
		color
	);
}

void buki::OpenGLGraphics::DrawSprite(
	std::uint32_t textureId,
	float textureWidthPixels,
	float textureHeightPixels,
	const Camera2D& camera,
	const glm::vec2& worldPositionMeters,
	float widthMeters,
	float heightMeters,
	const RectF& sourceRectPixels,
	float rotationRadians,
	bool flipX,
	bool flipY,
	const Color& color)
{
	if (textureId == 0 || textureWidthPixels <= 0.0f || textureHeightPixels <= 0.0f)
	{
		return;
	}

	const UVRect uvRect = BuildUVRect(
		sourceRectPixels,
		textureWidthPixels,
		textureHeightPixels,
		flipX,
		flipY
	);

	DrawTexturedQuad(
		textureId,
		camera,
		worldPositionMeters,
		widthMeters,
		heightMeters,
		rotationRadians,
		uvRect,
		color
	);
}

void buki::OpenGLGraphics::DrawSprite(
	const Texture2D& texture,
	const RectF& worldRectMeters,
	const RectF& sourceRectPixels,
	float rotationRadians,
	bool flipX,
	bool flipY)
{
	DrawSprite(
		texture,
		buki::Engine::Get().GetActiveCamera(),
		{ worldRectMeters.x, worldRectMeters.y },
		worldRectMeters.w,
		worldRectMeters.h,
		sourceRectPixels,
		rotationRadians,
		flipX,
		flipY
	);
}

void buki::OpenGLGraphics::DrawSprite(
	const Texture2D& texture,
	const RectF& worldRectMeters,
	const RectF& sourceRectPixels,
	float rotationRadians,
	bool flipX,
	bool flipY,
	const Color& color)
{
	DrawSprite(
		texture,
		buki::Engine::Get().GetActiveCamera(),
		{ worldRectMeters.x, worldRectMeters.y },
		worldRectMeters.w,
		worldRectMeters.h,
		sourceRectPixels,
		rotationRadians,
		flipX,
		flipY,
		color
	);
}

void buki::OpenGLGraphics::DestroyFont(Font2D& font)
{
	if (font.textureId != 0)
	{
		DestroyTexture(font.textureId);
	}

	font.textureId = 0;
	font.atlasWidth = 0;
	font.atlasHeight = 0;
	font.fontSize = 0;
	font.lineHeight = 0;
	font.ascent = 0;
	font.lineSkip = 0;
	font.valid = false;
	font.glyphs.clear();
}

buki::Font2D buki::OpenGLGraphics::CreateFontFromFile(const std::string& path, int fontSize)
{
	Font2D font{};

	if (fontSize <= 0)
	{
		Engine::Get().Log().LogWarning("Invalid font size: " + path);
		return font;
	}

	if (!TTF_WasInit() && !TTF_Init())
	{
		Engine::Get().Log().LogWarning("TTF_OpenFont failed \n" + path);
		return font;
	}

	using TtfFontPtr = std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>;
	using SurfacePtr = std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)>;

	TtfFontPtr ttf(TTF_OpenFont(path.c_str(), static_cast<float>(fontSize)), TTF_CloseFont);

	if (!ttf)
	{
		Engine::Get().Log().LogWarning("TTF_OpenFont failed: " + path);
		return font;
	}

	font.fontSize = fontSize;
	font.lineHeight = TTF_GetFontHeight(ttf.get());
	font.ascent = TTF_GetFontAscent(ttf.get());
	font.lineSkip = TTF_GetFontLineSkip(ttf.get());

	if (font.lineSkip <= 0)
	{
		font.lineSkip = font.lineHeight;
	}

	constexpr int firstChar = 32;
	constexpr int lastChar = 126;
	constexpr int glyphPadding = 1;

	struct TempGlyph
	{
		char character = 0;
		Glyph2D glyph{};
		std::vector<std::uint8_t> pixels;
	};

	std::vector<TempGlyph> tempGlyphs;
	tempGlyphs.reserve(lastChar - firstChar + 1);

	int atlasWidth = glyphPadding;
	int atlasHeight = 1;

	for (int character = firstChar; character <= lastChar; ++character)
	{
		int minX = 0;
		int maxX = 0;
		int minY = 0;
		int maxY = 0;
		int advance = 0;

		if (!TTF_GetGlyphMetrics(
			ttf.get(),
			static_cast<std::uint32_t>(character),
			&minX,
			&maxX,
			&minY,
			&maxY,
			&advance))
		{
			continue;
		}

		TempGlyph temp{};
		temp.character = static_cast<char>(character);
		temp.glyph.advance = advance;

		const SDL_Color white{ 255, 255, 255, 255 };

		SurfacePtr rendered(
			TTF_RenderGlyph_Blended(ttf.get(), static_cast<std::uint32_t>(character), white),
			SDL_DestroySurface);

		if (!rendered)
		{
			// Preserve advance even when there is no drawable bitmap.
			tempGlyphs.push_back(std::move(temp));
			continue;
		}

		SurfacePtr surface(
			SDL_ConvertSurface(rendered.get(), SDL_PIXELFORMAT_RGBA32),
			SDL_DestroySurface);

		if (!surface)
		{
			Engine::Get().Log().LogWarning("Font glyph conversion failed: " + path);
			return Font2D{};
		}

		if (!SDL_LockSurface(surface.get()))
		{
			Engine::Get().Log().LogWarning("Font glyph surface lock failed: " + path);
			return Font2D{};
		}

		int cropLeft = surface->w;
		int cropTop = surface->h;
		int cropRight = -1;
		int cropBottom = -1;

		for (int y = 0; y < surface->h; ++y)
		{
			const std::uint8_t* row = static_cast<const std::uint8_t*>(surface->pixels) + y * surface->pitch;

			for (int x = 0; x < surface->w; ++x)
			{
				if (row[x * 4 + 3] == 0)
				{
					continue;
				}

				cropLeft = std::min(cropLeft, x);
				cropTop = std::min(cropTop, y);
				cropRight = std::max(cropRight, x);
				cropBottom = std::max(cropBottom, y);
			}
		}

		if (cropRight >= cropLeft && cropBottom >= cropTop)
		{
			Glyph2D& glyph = temp.glyph;

			glyph.width = cropRight - cropLeft + 1;
			glyph.height = cropBottom - cropTop + 1;

			// The cropped bitmap's position relative to the baseline.
			glyph.bearingX = minX + cropLeft;
			glyph.bearingY = font.ascent - cropTop;

			// Raster bounds for the bitmap actually stored in the atlas.
			glyph.minX = glyph.bearingX;
			glyph.maxX = glyph.bearingX + glyph.width;
			glyph.maxY = glyph.bearingY;
			glyph.minY = glyph.bearingY - glyph.height;

			const std::size_t pixelCount = static_cast<std::size_t>(glyph.width) * static_cast<std::size_t>(glyph.height) * 4;
			temp.pixels.resize(pixelCount);

			for (int y = 0; y < glyph.height; ++y)
			{
				const std::uint8_t* source = static_cast<const std::uint8_t*>(surface->pixels) + (cropTop + y) * surface->pitch + cropLeft * 4;
				std::uint8_t* destination = temp.pixels.data() + static_cast<std::size_t>(y) * glyph.width * 4;

				std::memcpy(destination, source, static_cast<std::size_t>(glyph.width) * 4);
			}

			atlasWidth += glyph.width + glyphPadding;
			atlasHeight = std::max(atlasHeight, glyph.height);
		}

		SDL_UnlockSurface(surface.get());

		// Blank glyphs, including spaces, retain their advance.
		tempGlyphs.push_back(std::move(temp));
	}

	if (tempGlyphs.empty() || atlasWidth == glyphPadding)
	{
		Engine::Get().Log().LogWarning("Font produced no visible glyphs: " + path);
		return Font2D{};
	}

	atlasHeight += glyphPadding * 2;

	const std::size_t atlasByteCount = static_cast<std::size_t>(atlasWidth) * static_cast<std::size_t>(atlasHeight) * 4;
	std::vector<std::uint8_t> atlasPixels(atlasByteCount, 0);

	int atlasX = glyphPadding;

	for (TempGlyph& temp : tempGlyphs)
	{
		Glyph2D& glyph = temp.glyph;

		if (glyph.width > 0 && glyph.height > 0)
		{
			glyph.sourceRectPixels = RectF
			{
				static_cast<float>(atlasX),
				static_cast<float>(glyphPadding),
				static_cast<float>(glyph.width),
				static_cast<float>(glyph.height)
			};

			for (int y = 0; y < glyph.height; ++y)
			{
				const std::uint8_t* source = temp.pixels.data() + static_cast<std::size_t>(y) * glyph.width * 4;
				const std::size_t destinationOffset = (static_cast<std::size_t>(y + glyphPadding) * atlasWidth + atlasX) * 4;
				std::uint8_t* destination = atlasPixels.data() + destinationOffset;

				std::memcpy(destination, source, static_cast<std::size_t>(glyph.width) * 4);
			}

			atlasX += glyph.width + glyphPadding;
		}

		font.glyphs[temp.character] = glyph;
	}

	font.textureId = CreateTextureRGBA8(atlasPixels.data(), atlasWidth, atlasHeight);

	if (font.textureId == 0)
	{
		Engine::Get().Log().LogWarning("CreateTextureRGBA8 failed for font atlas: " + path);
		return Font2D{};
	}

	font.atlasWidth = atlasWidth;
	font.atlasHeight = atlasHeight;
	font.valid = true;

	return font;
}

void buki::OpenGLGraphics::DrawTextToCamera(
	const Font2D& font,
	const std::string& text,
	const Camera2D& camera,
	const glm::vec2& worldPositionMeters,
	const Color& color,
	bool centerX,
	bool centerY,
	bool centerVisuallyY)
{
	if (!font.IsValid() || text.empty() || font.lineHeight <= 0)
	{
		return;
	}

	const float worldUnitsPerFontPixel = 1.0f / METRES_TO_PIXELS;
	const float pixelScale = WorldToScreenSize(worldUnitsPerFontPixel, camera);

	if (pixelScale <= 0.0f)
	{
		return;
	}

	const float lineSkip = static_cast<float>(font.lineSkip > 0 ? font.lineSkip : font.lineHeight);
	const float ascent = static_cast<float>(font.ascent);

	float currentLineWidth = 0.0f;
	float maximumLineWidth = 0.0f;
	float lineTop = 0.0f;
	int lineCount = 1;

	float visibleLeft = std::numeric_limits<float>::max();
	float visibleTop = std::numeric_limits<float>::max();
	float visibleRight = std::numeric_limits<float>::lowest();
	float visibleBottom = std::numeric_limits<float>::lowest();

	bool hasVisibleGlyph = false;

	for (char character : text)
	{
		if (character == '\r')
		{
			continue;
		}

		if (character == '\n')
		{
			maximumLineWidth = std::max(maximumLineWidth, currentLineWidth);
			currentLineWidth = 0.0f;
			lineTop += lineSkip;
			++lineCount;
			continue;
		}

		const Glyph2D* glyph = font.GetGlyph(character);

		if (!glyph)
		{
			continue;
		}

		if (glyph->width > 0 && glyph->height > 0)
		{
			const float left = currentLineWidth + static_cast<float>(glyph->bearingX);
			const float top = lineTop + ascent - static_cast<float>(glyph->bearingY);
			const float right = left + static_cast<float>(glyph->width);
			const float bottom = top + static_cast<float>(glyph->height);

			visibleLeft = std::min(visibleLeft, left);
			visibleTop = std::min(visibleTop, top);
			visibleRight = std::max(visibleRight, right);
			visibleBottom = std::max(visibleBottom, bottom);

			hasVisibleGlyph = true;
		}

		currentLineWidth += static_cast<float>(glyph->advance);
	}

	maximumLineWidth = std::max(maximumLineWidth, currentLineWidth);

	const float layoutHeight = static_cast<float>(font.lineHeight) + static_cast<float>(lineCount - 1) * lineSkip;
	const float layoutOffsetX = centerX ? -maximumLineWidth * 0.5f : 0.0f;

	float layoutOffsetY = centerY ? -layoutHeight * 0.5f : 0.0f;

	if (centerVisuallyY && hasVisibleGlyph)
	{
		layoutOffsetY = -(visibleTop + visibleBottom) * 0.5f;
	}

	const glm::vec2 screenAnchor = WorldToScreen(worldPositionMeters, camera);

#ifdef BUKI_DEBUG_TEXT_LAYOUT
	const auto drawDebugBounds = [&](float left, float top, float right, float bottom, const Color& debugColor)
		{
			const float screenLeft = screenAnchor.x + (layoutOffsetX + left) * pixelScale;
			const float screenTop = screenAnchor.y + (layoutOffsetY + top) * pixelScale;
			const float screenRight = screenAnchor.x + (layoutOffsetX + right) * pixelScale;
			const float screenBottom = screenAnchor.y + (layoutOffsetY + bottom) * pixelScale;

			DrawPrimitiveInternal(
				{
					{ screenLeft, screenTop },
					{ screenRight, screenTop },
					{ screenRight, screenBottom },
					{ screenLeft, screenBottom },
					{ screenLeft, screenTop }
				},
				debugColor,
				GL_LINE_STRIP);
		};

	drawDebugBounds(0.0f, 0.0f, maximumLineWidth, layoutHeight, Color{ 1.0f, 0.0f, 0.0f, 1.0f });

	if (hasVisibleGlyph)
	{
		drawDebugBounds(visibleLeft, visibleTop, visibleRight, visibleBottom, Color{ 0.0f, 1.0f, 0.0f, 1.0f });
	}
#endif

	float penX = 0.0f;
	float penY = 0.0f;

	for (char character : text)
	{
		if (character == '\r')
		{
			continue;
		}

		if (character == '\n')
		{
			penX = 0.0f;
			penY += lineSkip;
			continue;
		}

		const Glyph2D* glyph = font.GetGlyph(character);

		if (!glyph)
		{
			continue;
		}

		if (glyph->width > 0 && glyph->height > 0)
		{
			const float glyphLeft = layoutOffsetX + penX + static_cast<float>(glyph->bearingX);
			const float glyphTop = layoutOffsetY + penY + ascent - static_cast<float>(glyph->bearingY);

			const glm::vec2 glyphCenterScreen =
			{
				screenAnchor.x + (glyphLeft + static_cast<float>(glyph->width) * 0.5f) * pixelScale,
				screenAnchor.y + (glyphTop + static_cast<float>(glyph->height) * 0.5f) * pixelScale
			};

			const glm::vec2 glyphCenterWorld = ScreenToWorld(glyphCenterScreen, camera);

			const UVRect uvRect = BuildUVRect(
				glyph->sourceRectPixels,
				static_cast<float>(font.atlasWidth),
				static_cast<float>(font.atlasHeight),
				false,
				false);

			DrawTexturedQuad(
				font.textureId,
				camera,
				glyphCenterWorld,
				static_cast<float>(glyph->width) * worldUnitsPerFontPixel,
				static_cast<float>(glyph->height) * worldUnitsPerFontPixel,
				0.0f,
				uvRect,
				color);
		}

		penX += static_cast<float>(glyph->advance);
	}
}

buki::Vector2 buki::OpenGLGraphics::MeasureText(const Font2D& font, const std::string& text) const
{
	if (!font.IsValid() || text.empty())
	{
		return Vector2{ 0.0f, 0.0f };
	}

	float currentLineWidth = 0.0f;
	float maximumLineWidth = 0.0f;
	int lineCount = 1;

	for (char character : text)
	{
		if (character == '\r')
		{
			continue;
		}

		if (character == '\n')
		{
			maximumLineWidth = std::max(maximumLineWidth, currentLineWidth);
			currentLineWidth = 0.0f;
			++lineCount;
			continue;
		}

		const Glyph2D* glyph = font.GetGlyph(character);

		if (glyph)
		{
			currentLineWidth += static_cast<float>(glyph->advance);
		}
	}

	maximumLineWidth = std::max(maximumLineWidth, currentLineWidth);

	const float lineHeight = static_cast<float>(font.lineHeight);
	const float lineSkip = static_cast<float>(font.lineSkip > 0 ? font.lineSkip : font.lineHeight);
	const float layoutHeight = lineHeight + static_cast<float>(lineCount - 1) * lineSkip;

	return Vector2
	{
		maximumLineWidth / METRES_TO_PIXELS,
		layoutHeight / METRES_TO_PIXELS
	};
}

float buki::OpenGLGraphics::MeasureTextWidthPixels(const Font2D& font, const std::string& text) const
{
	float currentLineWidth = 0.0f;
	float maximumLineWidth = 0.0f;

	for (char character : text)
	{
		if (character == '\r')
		{
			continue;
		}

		if (character == '\n')
		{
			maximumLineWidth = std::max(maximumLineWidth, currentLineWidth);

			currentLineWidth = 0.0f;
			continue;
		}

		const Glyph2D* glyph = font.GetGlyph(character);

		if (glyph)
		{
			currentLineWidth += static_cast<float>(glyph->advance);
		}
	}
	return std::max(maximumLineWidth, currentLineWidth);
}

bool buki::OpenGLGraphics::InitializeLoader(IPlatform& platform)
{
	g_OpenGLPlatform = &platform;
	return gladLoadGLLoader((GLADloadproc)LoadOpenGLProc) != 0;
}

bool buki::OpenGLGraphics::CreateShaderProgram()
{
	const std::uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
	if (vertexShader == 0)
	{
		return false;
	}

	if (!CompileShader(vertexShader, kVertexShaderSource))
	{
		glDeleteShader(vertexShader);
		return false;
	}

	const std::uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (fragmentShader == 0)
	{
		glDeleteShader(vertexShader);
		return false;
	}

	if (!CompileShader(fragmentShader, kFragmentShaderSource))
	{
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return false;
	}

	m_ShaderProgram = glCreateProgram();
	if (m_ShaderProgram == 0)
	{
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return false;
	}

	glAttachShader(m_ShaderProgram, vertexShader);
	glAttachShader(m_ShaderProgram, fragmentShader);

	const bool linked = LinkProgram(m_ShaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	if (!linked)
	{
		glDeleteProgram(m_ShaderProgram);
		m_ShaderProgram = 0;
		return false;
	}

	m_ProjectionLocation = glGetUniformLocation(m_ShaderProgram, "uProjection");
	m_TextureLocation = glGetUniformLocation(m_ShaderProgram, "uTexture");

	return m_ProjectionLocation >= 0;
}

bool buki::OpenGLGraphics::CreateQuadBuffers()
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	if (m_VAO == 0 || m_VBO == 0 || m_EBO == 0)
	{
		DestroyQuadBuffers();
		return false;
	}

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, nullptr, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kQuadIndices), kQuadIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, x)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, r)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, u)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

void buki::OpenGLGraphics::DestroyShaderProgram()
{
	if (m_ShaderProgram != 0)
	{
		glDeleteProgram(m_ShaderProgram);
		m_ShaderProgram = 0;
	}
}

void buki::OpenGLGraphics::DestroyQuadBuffers()
{
	if (m_EBO != 0)
	{
		glDeleteBuffers(1, &m_EBO);
		m_EBO = 0;
	}

	if (m_VBO != 0)
	{
		glDeleteBuffers(1, &m_VBO);
		m_VBO = 0;
	}

	if (m_VAO != 0)
	{
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
}

void buki::OpenGLGraphics::UpdateProjection()
{
	m_Projection = glm::ortho(
		0.0f,
		static_cast<float>(m_ViewportWidth),
		static_cast<float>(m_ViewportHeight),
		0.0f,
		-1.0f,
		1.0f
	);
}

buki::UVRect buki::OpenGLGraphics::BuildUVRect(
	const RectF& sourceRectPixels,
	float textureWidthPixels,
	float textureHeightPixels,
	bool flipX,
	bool flipY) const
{
	UVRect uvRect{};
	uvRect.u0 = sourceRectPixels.x / textureWidthPixels;
	uvRect.v0 = sourceRectPixels.y / textureHeightPixels;
	uvRect.u1 = (sourceRectPixels.x + sourceRectPixels.w) / textureWidthPixels;
	uvRect.v1 = (sourceRectPixels.y + sourceRectPixels.h) / textureHeightPixels;

	if (flipX)
	{
		std::swap(uvRect.u0, uvRect.u1);
	}

	if (flipY)
	{
		std::swap(uvRect.v0, uvRect.v1);
	}

	return uvRect;
}

void buki::OpenGLGraphics::UploadQuadVertices(
	const glm::vec2& worldPositionMeters,
	float widthMeters,
	float heightMeters,
	float rotationRadians,
	const Color& color,
	const UVRect& uvRect,
	const Camera2D& camera) const
{
	const glm::vec2 positionPixels = WorldToScreen(worldPositionMeters, camera);

	const float widthPixels = WorldToScreenSize(widthMeters, camera);
	const float heightPixels = WorldToScreenSize(heightMeters, camera);

	const float halfWidth = widthPixels * 0.5f;
	const float halfHeight = heightPixels * 0.5f;

	const float screenRotation = WorldToScreenRotation(rotationRadians);
	const float cosine = std::cos(screenRotation);
	const float sine = std::sin(screenRotation);

	const auto rotate = [cosine, sine](const glm::vec2& point) -> glm::vec2
		{
			return
			{
				point.x * cosine - point.y * sine,
				point.x * sine + point.y * cosine
			};
		};

	const glm::vec2 p0 = positionPixels + rotate({ -halfWidth, -halfHeight });
	const glm::vec2 p1 = positionPixels + rotate({ halfWidth, -halfHeight });
	const glm::vec2 p2 = positionPixels + rotate({ halfWidth, halfHeight });
	const glm::vec2 p3 = positionPixels + rotate({ -halfWidth, halfHeight });

	const Vertex vertices[4] =
	{
		{ p0.x, p0.y, color.r, color.g, color.b, color.a, uvRect.u0, uvRect.v0 },
		{ p1.x, p1.y, color.r, color.g, color.b, color.a, uvRect.u1, uvRect.v0 },
		{ p2.x, p2.y, color.r, color.g, color.b, color.a, uvRect.u1, uvRect.v1 },
		{ p3.x, p3.y, color.r, color.g, color.b, color.a, uvRect.u0, uvRect.v1 }
	};

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool buki::OpenGLGraphics::CompileShader(std::uint32_t shader, const char* source) const
{
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	int success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (success != GL_TRUE)
	{
		int logLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

		std::string log;
		log.resize(static_cast<std::size_t>(std::max(logLength, 1)));

		glGetShaderInfoLog(shader, logLength, nullptr, log.data());

		OutputDebugStringA("Shader compile failed:\n");
		OutputDebugStringA(log.c_str());
		OutputDebugStringA("\n");
	}

	return success == GL_TRUE;
}

bool buki::OpenGLGraphics::LinkProgram(std::uint32_t program) const
{
	glLinkProgram(program);

	int success = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (success != GL_TRUE)
	{
		int logLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

		std::string log;
		log.resize(static_cast<std::size_t>(std::max(logLength, 1)));

		glGetProgramInfoLog(program, logLength, nullptr, log.data());

		OutputDebugStringA("Program link failed:\n");
		OutputDebugStringA(log.c_str());
		OutputDebugStringA("\n");
	}

	return success == GL_TRUE;
}

bool buki::OpenGLGraphics::CreateDebugLinePipeline()
{
	const std::uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
	if (vertexShader == 0) return false;
	if (!CompileShader(vertexShader, kDebugLineVertexShaderSource))
	{
		glDeleteShader(vertexShader);
		return false;
	}

	const std::uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (fragmentShader == 0)
	{
		glDeleteShader(vertexShader);
		return false;
	}
	if (!CompileShader(fragmentShader, kDebugLineFragmentShaderSource))
	{
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return false;
	}

	m_DebugLineProgram = glCreateProgram();
	if (m_DebugLineProgram == 0)
	{
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return false;
	}

	glAttachShader(m_DebugLineProgram, vertexShader);
	glAttachShader(m_DebugLineProgram, fragmentShader);

	const bool linked = LinkProgram(m_DebugLineProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	if (!linked)
	{
		glDeleteProgram(m_DebugLineProgram);
		m_DebugLineProgram = 0;
		return false;
	}

	m_DebugLineProjLoc = glGetUniformLocation(m_DebugLineProgram, "uProjection");
	m_DebugLineColorLoc = glGetUniformLocation(m_DebugLineProgram, "uColor");

	glGenVertexArrays(1, &m_DebugLineVAO);
	glGenBuffers(1, &m_DebugLineVBO);

	if (m_DebugLineVAO == 0 || m_DebugLineVBO == 0)
	{
		DestroyDebugLinePipeline();
		return false;
	}

	glBindVertexArray(m_DebugLineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_DebugLineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 1024, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

void buki::OpenGLGraphics::DestroyDebugLinePipeline()
{
	if (m_DebugLineVBO != 0)
	{
		glDeleteBuffers(1, &m_DebugLineVBO);
		m_DebugLineVBO = 0;
	}

	if (m_DebugLineVAO != 0)
	{
		glDeleteVertexArrays(1, &m_DebugLineVAO);
		m_DebugLineVAO = 0;
	}

	if (m_DebugLineProgram != 0)
	{
		glDeleteProgram(m_DebugLineProgram);
		m_DebugLineProgram = 0;
	}

	m_DebugLineProjLoc = -1;
	m_DebugLineColorLoc = -1;
}

void buki::OpenGLGraphics::DrawPrimitiveInternal(const std::vector<glm::vec2>& points, const Color& color, GLenum mode)
{
	if (points.empty() || m_DebugLineProgram == 0 || m_DebugLineVAO == 0 || m_DebugLineVBO == 0)
	{
		return;
	}

	std::vector<float> vertices;
	vertices.reserve(points.size() * 2);
	for (const glm::vec2& p : points)
	{
		vertices.push_back(p.x);
		vertices.push_back(p.y);
	}

	glUseProgram(m_DebugLineProgram);
	if (m_DebugLineProjLoc >= 0)
	{
		const glm::mat4 projection =
			GetCurrentViewportProjection();

		glUniformMatrix4fv(
			m_DebugLineProjLoc,
			1,
			GL_FALSE,
			glm::value_ptr(projection));
	}
	if (m_DebugLineColorLoc >= 0)
	{
		glUniform4f(m_DebugLineColorLoc, color.r, color.g, color.b, color.a);
	}

	glBindVertexArray(m_DebugLineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_DebugLineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
	glDrawArrays(mode, 0, static_cast<GLsizei>(points.size()));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

float buki::OpenGLGraphics::GetLineThicknessPixels(const LineStyle& lineStyle, const Camera2D& camera) const
{
	if (!std::isfinite(lineStyle.thickness) || lineStyle.thickness <= 0.0f)
	{
		return 0.0f;
	}

	if (lineStyle.space == LineThicknessSpace::WorldUnits)
	{
		return std::abs(WorldToScreenSize(lineStyle.thickness, camera));
	}

	return lineStyle.thickness;
}

void buki::OpenGLGraphics::DrawStrokeInternal(
	const std::vector<glm::vec2>& points,
	bool closed,
	const Color& color,
	float thicknessPixels)
{
	constexpr float epsilon = 0.00001f;

	if (!std::isfinite(thicknessPixels) || thicknessPixels <= 0.0f)
	{
		return;
	}

	const float halfThickness = thicknessPixels * 0.5f;

	if (!closed)
	{
		if (points.size() != 2)
		{
			return;
		}

		const glm::vec2 delta = points[1] - points[0];
		const float length = glm::length(delta);

		if (length <= epsilon)
		{
			return;
		}

		const glm::vec2 direction = delta / length;
		const glm::vec2 normal{ -direction.y, direction.x };
		const glm::vec2 offset = normal * halfThickness;

		const glm::vec2 a = points[0] + offset;
		const glm::vec2 b = points[1] + offset;
		const glm::vec2 c = points[1] - offset;
		const glm::vec2 d = points[0] - offset;

		const std::vector<glm::vec2> triangles =
		{
			a, b, c,
			a, c, d
		};

		DrawPrimitiveInternal(triangles, color, GL_TRIANGLES);
		return;
	}

	const std::size_t count = points.size();

	if (count < 3)
	{
		return;
	}

	float signedDoubleArea = 0.0f;
	glm::vec2 center{ 0.0f, 0.0f };

	for (std::size_t i = 0; i < count; ++i)
	{
		const glm::vec2& a = points[i];
		const glm::vec2& b = points[(i + 1) % count];

		signedDoubleArea += a.x * b.y - b.x * a.y;
		center += a;
	}

	if (std::abs(signedDoubleArea) <= epsilon)
	{
		return;
	}

	center /= static_cast<float>(count);

	const float orientation = signedDoubleArea > 0.0f ? 1.0f : -1.0f;

	std::vector<glm::vec2> inwardNormals(count);
	float minimumCenterDistance = std::numeric_limits<float>::max();

	for (std::size_t i = 0; i < count; ++i)
	{
		const glm::vec2 delta = points[(i + 1) % count] - points[i];
		const float length = glm::length(delta);

		if (length <= epsilon)
		{
			return;
		}

		const glm::vec2 direction = delta / length;
		const glm::vec2 inward = glm::vec2{ -direction.y, direction.x } *orientation;

		inwardNormals[i] = inward;

		const float centerDistance = glm::dot(center - points[i], inward);
		minimumCenterDistance = std::min(minimumCenterDistance, centerDistance);
	}

	std::vector<glm::vec2> innerPoints(count);
	std::vector<glm::vec2> outerPoints(count);

	for (std::size_t i = 0; i < count; ++i)
	{
		const glm::vec2 previousNormal = inwardNormals[(i + count - 1) % count];
		const glm::vec2 currentNormal = inwardNormals[i];
		const glm::vec2 normalSum = previousNormal + currentNormal;

		const float denominator = glm::dot(normalSum, currentNormal);

		if (denominator <= epsilon)
		{
			return;
		}

		const glm::vec2 offset = normalSum * (halfThickness / denominator);

		innerPoints[i] = points[i] + offset;
		outerPoints[i] = points[i] - offset;
	}

	std::vector<glm::vec2> triangles;
	triangles.reserve(count * 6);

	// For rectangles and regular polygons, this is where
	// the inward stroke consumes the remaining interior.
	if (halfThickness >= minimumCenterDistance)
	{
		for (std::size_t i = 0; i < count; ++i)
		{
			const std::size_t next = (i + 1) % count;

			triangles.push_back(center);
			triangles.push_back(outerPoints[i]);
			triangles.push_back(outerPoints[next]);
		}
	}
	else
	{
		for (std::size_t i = 0; i < count; ++i)
		{
			const std::size_t next = (i + 1) % count;

			triangles.push_back(outerPoints[i]);
			triangles.push_back(outerPoints[next]);
			triangles.push_back(innerPoints[next]);

			triangles.push_back(outerPoints[i]);
			triangles.push_back(innerPoints[next]);
			triangles.push_back(innerPoints[i]);
		}
	}

	DrawPrimitiveInternal(triangles, color, GL_TRIANGLES);
}
