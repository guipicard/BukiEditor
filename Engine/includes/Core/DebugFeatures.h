#pragma once

#ifndef BUKI_ENABLE_DEBUG_UI
#if defined(_DEBUG)
#define BUKI_ENABLE_DEBUG_UI 1
#else
#define BUKI_ENABLE_DEBUG_UI 0
#endif
#endif


//#include "DebugFeatures.h"
//
//#if BUKI_ENABLE_DEBUG_UI
//#include "imgui.h"
//#endif

//#if BUKI_ENABLE_DEBUG_UI
//
//// Runtime debug windows and registrations.
//
//#endif