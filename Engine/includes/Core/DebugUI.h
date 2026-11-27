#pragma once

#include "DebugFeatures.h"

#if BUKI_ENABLE_DEBUG_UI

#include <functional>
#include <string>
#include <vector>

namespace buki
{
    enum class DebugMenuGroup
    {
        Engine,
        Game
    };

    class DebugUI
    {
    public:
        using DrawCallback = std::function<void()>;

        bool Initialize(void* sdlWindow, void* glContext);
        void Shutdown();
        bool IsInitialized() const { return m_Initialized; }

        void ProcessEvent(const void* sdlEvent);
        void BeginFrame();
        void Render();

        bool RegisterWindow(const std::string& id, DebugMenuGroup group, const std::string& title, DrawCallback callback, const void* owner = nullptr, bool openByDefault = false);
        void UnregisterWindow(const std::string& id);
        void UnregisterOwner(const void* owner);

        bool IsVisible() const { return m_Visible; }
        bool IsGameInputBlocked() const { return m_InputBlocked; }

    private:
        struct Entry
        {
            std::string id;
            std::string title;
            DebugMenuGroup group = DebugMenuGroup::Engine;
            DrawCallback callback;
            const void* owner = nullptr;
            bool open = false;
        };

        void DrawDockspace();
        void DrawMenuBar();
        void DrawWindows();
        void DrawBlockedIndicator();

        std::vector<Entry> m_Windows;
        bool m_Initialized = false;
        bool m_Visible = false;
        bool m_InputBlocked = false;
    };
}

#endif