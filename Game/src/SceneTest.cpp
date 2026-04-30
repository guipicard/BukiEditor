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
	Entity* test = buki::Engine::Get().World().Create("test");
	OpenGlTest* opengl = test->AddComponent<OpenGlTest>();
}
void buki::SceneTest::OnWindowResize()
{
}

void buki::SceneTest::OnNotify(const std::string& button)
{
}
