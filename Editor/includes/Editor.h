#pragma once

#include <SDL3/SDL.h>

#include "EditorLayer.h"

namespace buki
{
	struct Editor
	{
		Editor();
		~Editor();

		bool Init();
		void Update(SDL_Event const& e);
		void BeginFrame();
		void Render();
		void Present();
		void Shutdown();

	private:
		float main_scale = 1.0f;
		float clearColor[4] = { 0.10f, 0.10f, 0.12f, 1.00f };

		SDL_Window* gameWindow = nullptr;
		SDL_GLContext gl_context = nullptr;

		EditorLayer editorLayer;
	};
}