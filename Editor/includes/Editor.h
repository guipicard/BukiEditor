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
		void Update(const SDL_Event& e);
		void BeginFrame();
		void Render();
		void Present();
		void Shutdown();

	private:
		float main_scale = 1.0f;
		float m_ClearColorR = 0.08f;
		float m_ClearColorG = 0.08f;
		float m_ClearColorB = 0.12f;
		float m_ClearColorA = 1.0f;
		float clearColor[4] = { m_ClearColorR, m_ClearColorG, m_ClearColorB, m_ClearColorA };

		SDL_Window* gameWindow = nullptr;

		EditorLayer editorLayer;
	};
}