#include "GameComponentRegistration.h"

#include "ComponentRegistration.h"

#include "AngryBirdController.h"
#include "BricksBehaviour.h"
#include "ShapesController.h"
#include "AnimTest.h"
#include "OpenGlTest.h"
#include "CameraFollow.h"

#include "ScriptFunctionRegistry.h"

void LoadMenu()
{
	buki::Engine::Get().World().SetLoadScene("Menu");
}

void LoadShapesDemo()
{
	buki::Engine::Get().World().SetLoadScene("Shapes");
}

void LoadAgMenu()
{
	buki::Engine::Get().World().SetLoadScene("AngryBird");
}

void LoadAgLevel1()
{
	buki::Engine::Get().World().SetLoadScene("Ag_Lvl_1");
}

void LoadAgLevel2()
{
	buki::Engine::Get().World().SetLoadScene("Ag_Lvl_2");
}

void LoadAgLevel3()
{
	buki::Engine::Get().World().SetLoadScene("Ag_Lvl_3");
}
void LoadCurrentScene()
{
	auto& world = buki::Engine::Get().World();
	world.SetLoadScene(world.GetCurrentSceneName());
}
void LoadPlatformer()
{
	buki::Engine::Get().World().SetLoadScene("Platformer");
}
void LoadSceneTest()
{
	buki::Engine::Get().World().SetLoadScene("SceneTest");
}
void Quit()
{
	buki::Engine::Get().Platform().RequestQuit();
}
void game::RegisterGameComponents()
{
	REGISTER_COMPONENT_EXPLICIT(buki::AngryBirdController, "AngryBirdController");
	REGISTER_COMPONENT_EXPLICIT(buki::BricksBehaviour, "BricksBehaviour");
	REGISTER_COMPONENT_EXPLICIT(buki::ShapesController, "ShapesController");
	REGISTER_COMPONENT_EXPLICIT(buki::AnimTest, "AnimTest");
	REGISTER_COMPONENT_EXPLICIT(buki::OpenGlTest, "OpenGlTest");
	REGISTER_COMPONENT_EXPLICIT(buki::CameraFollow, "CameraFollow");

	auto& registry = buki::ScriptFunctionRegistry::Get();
	registry.RegisterStaticVoidFunction("GameComponentRegistration", "LoadMenu", LoadMenu);
	registry.RegisterStaticVoidFunction("GameComponentRegistration", "LoadShapesDemo", LoadShapesDemo);
	registry.RegisterStaticVoidFunction("GameComponentRegistration", "LoadAgMenu", LoadAgMenu);
	registry.RegisterStaticVoidFunction("GameComponentRegistration", "LoadAgLevel1", LoadAgLevel1);
	registry.RegisterStaticVoidFunction("GameComponentRegistration", "LoadAgLevel2", LoadAgLevel2);
	registry.RegisterStaticVoidFunction("GameComponentRegistration", "LoadAgLevel3", LoadAgLevel3);
	registry.RegisterStaticVoidFunction("GameComponentRegistration", "Reset", LoadCurrentScene);
	registry.RegisterStaticVoidFunction("GameComponentRegistration", "LoadPlatformer", LoadPlatformer);
	registry.RegisterStaticVoidFunction("GameComponentRegistration", "LoadSceneTest", LoadSceneTest);
	registry.RegisterStaticVoidFunction("GameComponentRegistration", "Quit", Quit);

	registry.RegisterStaticVoidFunction("ShapesController", "DrawBoxFill", buki::ShapesController::DrawBoxFill);
	registry.RegisterStaticVoidFunction("ShapesController", "DrawBoxShape", buki::ShapesController::DrawBoxShape);
	registry.RegisterStaticVoidFunction("ShapesController", "DrawBoxBoth", buki::ShapesController::DrawBoxBoth);

	registry.RegisterStaticVoidFunction("ShapesController", "DrawCircleFill", buki::ShapesController::DrawCircleFill);
	registry.RegisterStaticVoidFunction("ShapesController", "DrawCircleShape", buki::ShapesController::DrawCircleShape);
	registry.RegisterStaticVoidFunction("ShapesController", "DrawCircleBoth", buki::ShapesController::DrawCircleBoth);

	registry.RegisterStaticVoidFunction("ShapesController", "DrawPolygonFill", buki::ShapesController::DrawPolygonFill);
	registry.RegisterStaticVoidFunction("ShapesController", "DrawPolygonShape", buki::ShapesController::DrawPolygonShape);
	registry.RegisterStaticVoidFunction("ShapesController", "DrawPolygonBoth", buki::ShapesController::DrawPolygonBoth);
}

