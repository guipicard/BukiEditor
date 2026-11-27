#include "Engine.h"
#include "DebugFeatures.h"

#if BUKI_ENABLE_DEBUG_UI
#include "imgui.h"
#endif

namespace game
{
    void RegisterGameDebugWindows();
}