#pragma once
#include "SDL_events.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
struct ImTextureRef;

namespace buki
{
	struct Editor
	{
		Editor();
		~Editor();
		bool Init();
		void Update(SDL_Event _event);
		void EditorClear();
		void Render();
		void EditorPresent();
		void Shutdown();

		inline void SetGameWindow(SDL_Window* _window) { gameWindow = _window; }
	private:
		float main_scale = 0;
		bool show_demo_window = true;
		bool show_another_window = false;
		float imguiColor[4] = { 0.45f, 0.55f, 0.60f, 1.00f };

		bool show_Game_Window = true;

		bool show_metrics_window = true;
		bool show_asset_browser_window = true;
		bool show_property_window = true;
		bool show_game_window = true;
		bool show_console_window = true;
		bool show_world_editor_window = true;
		bool show_entity_inspector_window = true;

		SDL_Window* window = nullptr;
		SDL_Window* gameWindow = nullptr;
		void* gl_context = nullptr;
		SDL_Renderer* renderer = nullptr;
	};
}
