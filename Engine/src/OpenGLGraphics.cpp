#pragma once
#define NOMINMAX
#include <Windows.h>

#include "Graphics/OpenGLGraphics.h"
#include "Platform/IPlatform.h"
#include "Graphics/Texture2D.h"
#include "Camera2DUtils.h"
#include "Units.h"
#include "Engine.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <string>

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

	glUseProgram(m_ShaderProgram);
	glUniformMatrix4fv(m_ProjectionLocation, 1, GL_FALSE, glm::value_ptr(m_Projection));

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

void buki::OpenGLGraphics::DrawLine(const Vector2& a, const Vector2& b, const Color& color)
{
	std::vector<glm::vec2> points = { {a.x, a.y}, {b.x, b.y} };
	DrawPrimitiveInternal(points, color, GL_LINES);
}

void buki::OpenGLGraphics::DrawRectOutline(const Vector2& center, const Vector2& size, float rotationRadians, const Color& color)
{
	const glm::vec2 screenCenter = WorldToScreen(glm::vec2{ center.x, center.y }, buki::Engine::Get().GetActiveCamera());
	const glm::vec2 screenSize = Units::ToPixels({ size.x, size.y });
	const Vector2 half = { screenSize.x * 0.5f, screenSize.y * 0.5f };

	auto rotate = [rotationRadians](const glm::vec2& p) -> glm::vec2
		{
			const float c = std::cos(rotationRadians);
			const float s = std::sin(rotationRadians);
			return
			{
				p.x * c - p.y * s,
				p.x * s + p.y * c
			};
		};

	std::vector<glm::vec2> points;
	points.reserve(4);

	points.push_back(screenCenter + rotate({ -half.x, -half.y }));
	points.push_back(screenCenter + rotate({ half.x, -half.y }));
	points.push_back(screenCenter + rotate({ half.x,  half.y }));
	points.push_back(screenCenter + rotate({ -half.x,  half.y }));

	DrawPrimitiveInternal(points, color, GL_LINE_LOOP);
}

void buki::OpenGLGraphics::FillRect(const Vector2& center, const Vector2& size, float rotationRadians, const Color& color)
{
	const glm::vec2 screenCenter = WorldToScreen(glm::vec2{ center.x, center.y }, buki::Engine::Get().GetActiveCamera());
	const glm::vec2 screenSize = Units::ToPixels({ size.x, size.y });
	const glm::vec2 half = { screenSize.x * 0.5f, screenSize.y * 0.5f };

	auto rotate = [rotationRadians](const glm::vec2& p) -> glm::vec2
		{
			const float c = std::cos(rotationRadians);
			const float s = std::sin(rotationRadians);
			return { p.x * c - p.y * s, p.x * s + p.y * c };
		};

	std::vector<glm::vec2> points;
	points.reserve(6);

	points.push_back(screenCenter);
	points.push_back(screenCenter + rotate({ -half.x, -half.y }));
	points.push_back(screenCenter + rotate({ half.x, -half.y }));
	points.push_back(screenCenter + rotate({ half.x,  half.y }));
	points.push_back(screenCenter + rotate({ -half.x,  half.y }));
	points.push_back(screenCenter + rotate({ -half.x, -half.y }));

	DrawPrimitiveInternal(points, color, GL_TRIANGLE_FAN);
}

void buki::OpenGLGraphics::DrawCircleOutline(const Vector2& center, float radius, const Color& color)
{
	const glm::vec2 screenCenter = WorldToScreen(glm::vec2{ center.x, center.y }, buki::Engine::Get().GetActiveCamera());

	const float screenRadius = Units::ToPixels(radius);
	int segments = std::clamp(static_cast<int>(screenRadius), 16, 128);

	std::vector<glm::vec2> points;
	points.reserve(static_cast<size_t>(segments));

	const float step = 6.28318530718f / static_cast<float>(segments);
	const float angleOffset = 6.28318530718f / 4;
	for (int i = 0; i < segments; ++i)
	{
		const float a = (static_cast<float>(i) * step) - angleOffset;
		points.push_back(
			{
				screenCenter.x + std::cos(a) * screenRadius,
				screenCenter.y + std::sin(a) * screenRadius
			});
	}
	DrawPrimitiveInternal({ points[0], screenCenter }, color, GL_LINES);
	DrawPrimitiveInternal(points, color, GL_LINE_LOOP);
}

void buki::OpenGLGraphics::FillCircle(const Vector2& center, float radius, const Color& color)
{
	const glm::vec2 screenCenter = WorldToScreen(glm::vec2{ center.x, center.y }, buki::Engine::Get().GetActiveCamera());

	const float screenRadius = Units::ToPixels(radius);
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

void buki::OpenGLGraphics::DrawPolygonOutline(const Vector2& center, float radius, float rotationRadians, const Color& color, int segments)
{
	if (segments < 3)
	{
		segments = 3;
	}
	const glm::vec2 screenCenter = WorldToScreen(glm::vec2{ center.x, center.y }, buki::Engine::Get().GetActiveCamera());
	const float screenRadius = Units::ToPixels(radius);
	std::vector<glm::vec2>& points = GetPolygonPoints(screenCenter, screenRadius, rotationRadians, segments, true);
	points.erase(points.begin());
	DrawPrimitiveInternal(points, color, GL_LINE_LOOP);
}

void buki::OpenGLGraphics::FillPolygon(const Vector2& center, float radius, float rotationRadians, const Color& color, int segments)
{
	if (segments < 3)
	{
		segments = 3;
	}
	const glm::vec2 screenCenter = WorldToScreen(glm::vec2{ center.x, center.y }, buki::Engine::Get().GetActiveCamera());
	const float screenRadius = Units::ToPixels(radius);
	const std::vector<glm::vec2>& points = GetPolygonPoints(screenCenter, screenRadius, rotationRadians, segments, true);
	DrawPrimitiveInternal(points, color, GL_TRIANGLE_FAN);
}

std::vector<glm::vec2> buki::OpenGLGraphics::GetPolygonPoints(glm::vec2 center, float radius, float rotationRadians, int segments, bool renderFill)
{
	const int additionnalSegments = renderFill ? 2 : 0;
	std::vector<glm::vec2> points;
	points.reserve(static_cast<size_t>(segments + additionnalSegments));
	if (renderFill)
	{
		points.push_back(center);
	}
	const float step = 6.28318530718f / static_cast<float>(segments);
	const float angleOffset = (6.28318530718f / 4);
	for (int i = 0; i < segments + (additionnalSegments - 1); ++i)
	{
		const float a = ((static_cast<float>(i) * step) - angleOffset) + rotationRadians;
		points.push_back(
			{
				center.x + std::cos(a) * radius,
				center.y + std::sin(a) * radius
			});
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

void buki::OpenGLGraphics::DrawSprite(const Texture2D& texture, const RectF& worldRectMeters, const RectF& sourceRectPixels, float rotationRadians, bool flipX, bool flipY, const Color& color)
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

	const glm::vec2 localTopLeft = { -halfWidth, -halfHeight };
	const glm::vec2 localTopRight = { halfWidth, -halfHeight };
	const glm::vec2 localBottomRight = { halfWidth,  halfHeight };
	const glm::vec2 localBottomLeft = { -halfWidth,  halfHeight };

	auto rotate = [rotationRadians](const glm::vec2& p) -> glm::vec2
		{
			const float c = std::cos(rotationRadians);
			const float s = std::sin(rotationRadians);
			return {
				p.x * c - p.y * s,
				p.x * s + p.y * c
			};
		};

	const glm::vec2 p0 = rotate(localTopLeft) + positionPixels;
	const glm::vec2 p1 = rotate(localTopRight) + positionPixels;
	const glm::vec2 p2 = rotate(localBottomRight) + positionPixels;
	const glm::vec2 p3 = rotate(localBottomLeft) + positionPixels;

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
		glUniformMatrix4fv(m_DebugLineProjLoc, 1, GL_FALSE, glm::value_ptr(m_Projection));
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
