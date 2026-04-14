#pragma once
#include "SceneTest.h"

#include "Engine.h"
#include "Entity.h"
#include "OpenGlTest.h"
#include "AnimTest.h"
#include "Animation.h"

buki::SceneTest::SceneTest()
{
}

void buki::SceneTest::CodeLoad()
{
	Entity* player = buki::Engine::Get().World().Create("player");
	player->GetTransform()->SetPosition({ 0.0f, 0.0f });
	RectF frame
	{
		0.0f, 0.0f,
		96.0f, 96.0f
	};
	player->GetTransform()->SetSize({ static_cast<float>((frame.w) * 5), static_cast<float>((frame.h) * 5) });
	//OpenGlTest* opengl = player->AddComponent<OpenGlTest>();
	AnimTest* animTest = player->AddComponent<AnimTest>();
	animTest->GetAnimation()->Play("idle");	
	//Entity* Ground = buki::Engine::Get().World().Create("ground");
}

void buki::SceneTest::OnWindowResize()
{
}

void buki::SceneTest::OnNotify(const std::string& button)
{
}
