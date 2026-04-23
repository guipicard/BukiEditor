#pragma once
#include "SceneTest.h"

#include "Engine.h"
#include "Entity.h"
#include "OpenGlTest.h"
#include "AnimTest.h"
#include "Animation.h"
#include "Box.h"
#include "RigidBody.h"
#include "Units.h"
#include "PhysicsDefs.h"
#include "BukiContainers.h"
#include <cmath>

buki::SceneTest::SceneTest()
{
}

void buki::SceneTest::CodeLoad()
{
	Vector2 playerPos, playerSize, groundPos, groundSize;
	playerPos = { 3.0f, 3.0f };
	playerSize = { 10.0f,10.0f };
	groundPos = { 0.0f,10.0f };
	groundSize = { 30.0f,1.0f };
	Entity* player = buki::Engine::Get().World().Create("player");
	player->T()->SetPosition(playerPos);
	//player->T()->SetRotation(50.0f * PI / 180.0f);
	player->T()->SetRotation(0.0f * PI /180.0f);
	player->T()->SetSize(playerSize);
	AnimTest* animTest = player->AddComponent<AnimTest>();

	Entity* test = buki::Engine::Get().World().Create("test");
	OpenGlTest* opengl = test->AddComponent<OpenGlTest>();

	Entity* ground = buki::Engine::Get().World().Create("ground");
	ground->T()->SetPosition(groundPos);
	ground->T()->SetSize(groundSize);

	RigidBody* groundRigidbody = ground->AddComponent<RigidBody>();
	Box* groundBox = ground->AddComponent<Box>();

	groundRigidbody->def.type = RigidBodyDef::BodyType::Static;

	groundBox->def.fillDraw = true;
	groundBox->def.shapeDraw = true;
	groundBox->def.fillColor = { 1.0f, 0.0f, 0.0f, 0.3f };
	groundBox->def.shapeColor = { 0.0f, 1.0f, 0.0f, 1.0f };
	groundBox->def.size = groundSize;

	ground->ActivatePhysics();
}
void buki::SceneTest::OnWindowResize()
{
}

void buki::SceneTest::OnNotify(const std::string& button)
{
}
