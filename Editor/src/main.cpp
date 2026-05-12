#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
#include <SDL3/SDL.h>
#include "Editor.h"
#include "GameComponentRegistration.h"
#include "EngineComponentRegistration.h"
#include "ComponentFactory.h"
#include <cassert>


INT WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PSTR, _In_ INT)
{
	buki::RegisterEngineComponents();
	game::RegisterGameComponents();
	assert(buki::ComponentFactory::IsRegistered("AngryBirdController"));
	assert(buki::ComponentFactory::IsRegistered("BricksBehaviour"));
	assert(buki::ComponentFactory::IsRegistered("ShapesController"));
	assert(buki::ComponentFactory::IsRegistered("AnimTest"));
	assert(buki::ComponentFactory::IsRegistered("OpenGlTest"));
	//if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
	//{
	//	return -1;
	//}

	buki::Editor editor;
	if (!editor.Init())
	{
		return -1;
	}

	bool running = true;
	while (running)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_EVENT_QUIT)
			{
				running = false;
			}

			editor.Update(e);
		}

		editor.BeginFrame();
		editor.Render();
		editor.Present();
	}

	editor.Shutdown();
	
	buki::ComponentFactory::Destroy();
	return 0;
}