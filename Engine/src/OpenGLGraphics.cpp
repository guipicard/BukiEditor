#pragma once
#include "Graphics/OpenGLGraphics.h"

#include "Platform/IPlatform.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Graphics/Texture2D.h"

namespace
{
	buki::IPlatform* g_OpenGLPlatform = nullptr;

	void* LoadOpenGLProc(const char* procName)
	{
		return g_OpenGLPlatform != nullptr
			? g_OpenGLPlatform->GetProcAddress(procName)
			: nullptr;
	}

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
		Shutdown();
		return false;
	}

	if (!CreateQuadBuffers())
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
	DestroyShaderProgram();

	DestroyTexture(m_WhiteTexture);
	m_WhiteTexture = 0;

	m_ProjectionLocation = -1;
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

void buki::OpenGLGraphics::DrawQuad(float x, float y,
	float width, float height,
	float rotationRadians,
	float originX, float originY,
	float u0, float v0, float u1, float v1,
	float r, float g, float b, float a)
{
	DrawTexturedQuad(
		m_WhiteTexture,
		x, y,
		width, height,
		rotationRadians,
		originX, originY,
		u0, v0, u1, v1,
		r, g, b, a);
}


std::uint32_t buki::OpenGLGraphics::CreateTextureRGBA8(const void* pixels, int width, int height)
{

	if (pixels == nullptr || width <= 0 || height <= 0)
	{
		return 0;
	}

	std::uint32_t textureId = 0;
	glGenTextures(1, &textureId);

	if (textureId == 0)
	{
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

void buki::OpenGLGraphics::DrawTexturedQuad(std::uint32_t textureId, 
	float x, float y, float width, float height, 
	float rotationRadians,
	float originX, float originY,
	float u0, float v0, float u1, float v1, 
	float r, float g, float b, float a)
{
	if (textureId == 0 || m_ShaderProgram == 0 || m_VAO == 0 || m_VBO == 0 || m_EBO == 0)
	{
		return;
	}

	UploadQuadVertices(
		x, y,
		width, height,
		rotationRadians,
		originX, originY,
		u0, v0, u1, v1,
		r, g, b, a);

	glUseProgram(m_ShaderProgram);
	glUniformMatrix4fv(m_ProjectionLocation, 1, GL_FALSE, glm::value_ptr(m_Projection));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glUniform1i(m_TextureLocation, 0);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void buki::OpenGLGraphics::SetCameraPosition(float x, float y)
{
	m_CameraPosition = { x, y };
}

void buki::OpenGLGraphics::ResetCamera()
{
	m_CameraPosition = { 0.0f, 0.0f };
}

void buki::OpenGLGraphics::DrawSprite(std::uint32_t textureId,
	float x, float y,
	float width, float height,
	const RectF& sourceRectPixels,
	float textureWidth, float textureHeight,
	float rotationRadians,
	float originX, float originY,
	float r, float g, float b, float a,
	bool flipX, bool flipY)
{
	if (textureWidth <= 0.0f || textureHeight <= 0.0f)
	{
		return;
	}

	const float u0 = sourceRectPixels.x / textureWidth;
	const float v0 = sourceRectPixels.y / textureHeight;
	const float u1 = (sourceRectPixels.x + sourceRectPixels.w) / textureWidth;
	const float v1 = (sourceRectPixels.y + sourceRectPixels.h) / textureHeight;

	DrawTexturedQuad(
		textureId,
		x, y,
		width, height,
		rotationRadians,
		originX, originY,
		u0, v0, u1, v1,
		r, g, b, a);
}

void buki::OpenGLGraphics::DrawSprite(const Texture2D& texture,
	float x, float y,
	float width, float height,
	const RectF& sourceRectPixels,
	float rotationRadians,
	float originX, float originY,
	float r, float g, float b, float a, 
	bool flipX, bool flipY)
{
	if (!texture.IsValid())
	{
		return;
	}

	DrawSprite(
		texture.id,
		x, y,
		width, height,
		sourceRectPixels,
		static_cast<float>(texture.width),
		static_cast<float>(texture.height),
		rotationRadians,
		originX, originY,
		r, g, b, a, 
		flipX, flipY);
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

	return m_ProjectionLocation >= 0 && m_TextureLocation >= 0;
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
		1.0f);
}

void buki::OpenGLGraphics::UploadQuadVertices(float x, float y, float width, float height,
	float rotationRadians,
	float originX, float originY,
	float u0, float v0, float u1, float v1,
	float r, float g, float b, float a)
{
	const glm::vec2 localTopLeft = { -originX,          -originY };
	const glm::vec2 localTopRight = { -originX + width,  -originY };
	const glm::vec2 localBottomRight = { -originX + width,  -originY + height };
	const glm::vec2 localBottomLeft = { -originX,          -originY + height };

	const float c = std::cos(rotationRadians);
	const float s = std::sin(rotationRadians);

	const auto rotate = [c, s](const glm::vec2& v) -> glm::vec2
		{
			return {
				v.x * c - v.y * s,
				v.x * s + v.y * c
			};
		};

	const glm::vec2 position = {
		x + originX - m_CameraPosition.x,
		y + originY - m_CameraPosition.y
	};

	const glm::vec2 p0 = rotate(localTopLeft) + position;
	const glm::vec2 p1 = rotate(localTopRight) + position;
	const glm::vec2 p2 = rotate(localBottomRight) + position;
	const glm::vec2 p3 = rotate(localBottomLeft) + position;

	const Vertex vertices[4] =
	{
		{ p0.x, p0.y, r, g, b, a, u0, v0 },
		{ p1.x, p1.y, r, g, b, a, u1, v0 },
		{ p2.x, p2.y, r, g, b, a, u1, v1 },
		{ p3.x, p3.y, r, g, b, a, u0, v1 }
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
	return success == GL_TRUE;
}

bool buki::OpenGLGraphics::LinkProgram(std::uint32_t program) const
{
	glLinkProgram(program);

	int success = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	return success == GL_TRUE;
}

