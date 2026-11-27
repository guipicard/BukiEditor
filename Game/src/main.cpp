#pragma once
#define win32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include<Windows.h>
#include "Engine.h"
#include "ComponentFactory.h"
#include "GameComponentRegistration.h"
#include "EngineComponentRegistration.h"
#include "GameDebugWindows.h"
#include <string>
#include <cstdio>
#include <set>
#include <filesystem>

using namespace buki;

static std::string GetSceneArgument()
{
	for (int i = 1; i + 1 < __argc; ++i)
	{
		if (std::string(__argv[i]) == "--scene")
		{
			return __argv[i + 1];
		}
	}

	return {};
}

void InitGameplay(const std::string& startupScene) {
	const char* registeredScenes[] = { "SceneTest", "Menu", "Shapes", "Platformer", "AngryBird", "Ag_Lvl_1", "Ag_Lvl_2", "Ag_Lvl_3", "Test" };

	std::set<std::string> registered;

	for (const char* sceneName : registeredScenes) {
		Engine::Get().World().Register(sceneName);
		registered.insert(sceneName);
	}

	std::string firstScene = "Menu";

	if (!startupScene.empty()) {
		const std::string stem = std::filesystem::path(startupScene).stem().string();

		if (registered.find(stem) == registered.end()) {
			Engine::Get().World().Register(stem);
		}

		firstScene = stem;
	}

	std::printf("[Game] Startup scene: %s\n", firstScene.c_str());
	Engine::Get().World().SetLoadScene(firstScene);
}

INT WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PSTR, _In_ INT) {
#if _DEBUG
	std::setvbuf(stdout, nullptr, _IONBF, 0);
#endif
	buki::RegisterEngineComponents();
	game::RegisterGameComponents();
	if (Engine::Get().Init("Buki Engine", 1920, 1080)) {
		game::RegisterGameDebugWindows();
		InitGameplay(GetSceneArgument());
		Engine::Get().Start();
		buki::ComponentFactory::Destroy();
	}
	return 0;
}
