#pragma once
#include "OpenGlTest.h"

#include "Animation.h"
#include "Box.h"
#include "RigidBody.h"
#include "Engine.h"
#include "Graphics/IGraphics.h"
#include "glm/glm.hpp"
#include "Graphics/Texture2D.h"

buki::OpenGlTest::OpenGlTest(Entity* entity)
	: MonoBehaviour(entity)
{
	Awake();
}

void buki::OpenGlTest::Awake()
{
	playerTexture = Textures().Load("./assets/Samurai/HURT.png");
	frame =
	{
		0.0f, 0.0f,
		96.0f, 96.0f
	};
	fullRect =
	{
		0.0f,
		0.0f,
		static_cast<float>(playerTexture->width),
		static_cast<float>(playerTexture->height)
	};
	
}

void buki::OpenGlTest::Start()
{
}

void buki::OpenGlTest::FixedUpdate(const float dt)
{
}

void buki::OpenGlTest::Destroy()
{
}

void buki::OpenGlTest::Update(const float dt)
{
}

void buki::OpenGlTest::Draw(const float alpha)
{

	const unsigned char pixels[] =
	{
		255,   0,   0, 255,
		  0, 255,   0, 255,
		  0,   0, 255, 255,
		255, 255,   0, 255
	};
	tex = Graphics().CreateTextureRGBA8(pixels, 2, 2);
	Graphics().DrawQuad(100.0f, 100.0f, 256.0f, 256.0f,
		0.0f,
		0.0f, 0.0f,
		uv[0], uv[1], uv[2], uv[3],
		1.0f, 1.0f, 1.0f, 1.0f);
	Graphics().DrawTexturedQuad(tex, 400.0f, 100.0f, 256.0f, 256.0f,
		0.0f,
		0.0f, 0.0f,
		uv[0], uv[1], uv[2], uv[3],
		1.0f, 1.0f, 1.0f, 1.0f);
	Graphics().DrawTexturedQuad(tex, 700.0f, 100.0f, 256.0f, 256.0f,
		glm::radians(45.0f),
		128.0f, 128.0f,
		uv[0], uv[1], uv[2], uv[3],
		0.5f, 1.0f, 1.0f, 1.0f);
	Graphics().DrawTexturedQuad(tex, 1000.0f, 100.0f, 256.0f, 256.0f,
		glm::radians(45.0f),
		0.0f, 0.0f,
		uv[0], uv[1], uv[2], uv[3],
		1.0f, 0.5f, 1.0f, 1.0f);
	Graphics().DrawTexturedQuad(tex, 1300.0f, 100.0f, 256.0f, 256.0f,
		0.0f,
		0.0f, 0.0f,
		uv[0], uv[1], uv[2], uv[3],
		1.0f, 1.0f, 0.5f, 1.0f);
	Graphics().DrawTexturedQuad(tex, 1600.0f, 100.0f, 256.0f, 256.0f,
		0.0f,
		0.0f, 0.0f,
		uv[0], uv[1], uv[2], uv[3],
		0.5f, 0.5f, 1.0f, 1.0f);

	RectF src{ 0.0f, 0.0f, 256.0f, 128.0f };
	Graphics().DrawSprite(
		tex,
		100.0f, 400.0f,
		src.w, src.h,
		src,
		256.0f, 256.0f,
		glm::radians(20.0f),
		16.0f, 16.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		false, false);

	Graphics().DrawSprite(
		*playerTexture,
		300.0f, 400.0f,
		static_cast<float>((playerTexture->width / 4) * 5),
		static_cast<float>((playerTexture->height) * 5),
		frame,
		0.0f,
		playerTexture->width * 0.5f,
		playerTexture->height * 0.5f,
		1.0f, 1.0f, 1.0f, 1.0f,
		false, false);
}

void buki::OpenGlTest::OnCollisionEnter(Entity* other)
{
}

void buki::OpenGlTest::OnCollisionExit(Entity* other)
{
}

void buki::OpenGlTest::OnCollisionHit(Entity* other)
{
}

json buki::OpenGlTest::Serialize()
{
	return json();
}

void buki::OpenGlTest::Deserialize(json _doc)
{
}

void buki::OpenGlTest::Set()
{
}