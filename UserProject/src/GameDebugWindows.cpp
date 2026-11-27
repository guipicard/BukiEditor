#include "GameDebugWindows.h"

void game::RegisterGameDebugWindows()
{
#if BUKI_ENABLE_DEBUG_UI
	buki::Engine::Get().Debug().RegisterWindow("game.example", buki::DebugMenuGroup::Game, "Example", []() { ImGui::TextUnformatted("Game-specific debug content goes here."); });
#endif
}
