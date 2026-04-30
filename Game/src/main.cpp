#define win32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include<Windows.h>
#include "Engine.h"
#include "ShapesDemo.h"
#include "AngryBirdDemo.h"
#include "MenuScene.h"
#include "AGLevel1.h"
#include "AGLevel2.h"
#include "AGLevel3.h"
#include "SceneTest.h"
#include "Platformer.h"
#include <string>

using namespace buki;

void InitGameplay(void) {
	Engine::Get().World().Register("SceneTest", new SceneTest());
	Engine::Get().World().Register("Menu", new MenuScene());
	Engine::Get().World().Register("Shapes", new ShapesDemo());
	Engine::Get().World().Register("Platformer", new Platformer());
	Engine::Get().World().Register("AngryBird", new AngryBirdDemo());

	Engine::Get().World().Register("Ag_Lvl_1", new AGLevel1());
	Engine::Get().World().Register("Ag_Lvl_2", new AGLevel2());
	Engine::Get().World().Register("Ag_Lvl_3", new AGLevel3());


	Engine::Get().World().SetLoadScene("Menu");
}

INT WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PSTR, _In_ INT) { {}
if (Engine::Get().Init("Buki Engine", 1920, 1080)) {
	InitGameplay();
	Engine::Get().Start();
}
return 0;
}
