#define win32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include<Windows.h>
#include "Engine.h"
#include "ComponentFactory.h"
#include "GameComponentRegistration.h"
#include "EngineComponentRegistration.h"
#include <string>

using namespace buki;

void InitGameplay(void) {
	Engine::Get().World().Register("SceneTest");
	Engine::Get().World().Register("Menu");
	Engine::Get().World().Register("Shapes");
	Engine::Get().World().Register("Platformer");
	Engine::Get().World().Register("AngryBird");
	Engine::Get().World().Register("Ag_Lvl_1");
	Engine::Get().World().Register("Ag_Lvl_2");
	Engine::Get().World().Register("Ag_Lvl_3");

	Engine::Get().World().SetLoadScene("Menu");
}

INT WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PSTR, _In_ INT) {
	
	buki::RegisterEngineComponents();
	game::RegisterGameComponents();
	if (Engine::Get().Init("Buki Engine", 1920, 1080)) {
		InitGameplay();
		Engine::Get().Start();
		buki::ComponentFactory::Destroy();
	}
	return 0;
}
