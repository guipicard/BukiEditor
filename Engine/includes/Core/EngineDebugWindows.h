#pragma once

#include "DebugFeatures.h"

#if BUKI_ENABLE_DEBUG_UI

namespace buki
{
    class DebugUI;

    void RegisterEngineDebugWindows(DebugUI& ui);
}

#endif