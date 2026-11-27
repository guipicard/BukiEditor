#include "DebugUI.h"

#if BUKI_ENABLE_DEBUG_UI

#include "Engine.h"
#include "EngineDebugWindows.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include <SDL3/SDL.h>
#include <algorithm>

bool buki::DebugUI::Initialize(void* sdlWindow, void* glContext)
{
    if (m_Initialized)
    {
        return true;
    }

    if (sdlWindow == nullptr || glContext == nullptr)
    {
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = "DebugUI.ini";

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;

    if (!ImGui_ImplSDL3_InitForOpenGL(static_cast<SDL_Window*>(sdlWindow), glContext))
    {
        ImGui::DestroyContext();
        return false;
    }

    if (!ImGui_ImplOpenGL3_Init("#version 330"))
    {
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        return false;
    }

    m_Initialized = true;
    RegisterEngineDebugWindows(*this);
    return true;
}

void buki::DebugUI::Shutdown()
{
    if (!m_Initialized)
    {
        return;
    }

    m_Windows.clear();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    Engine::Get().Input().SetGameplayInputBlocked(false, false);

    m_Initialized = false;
    m_Visible = false;
    m_InputBlocked = false;
}

void buki::DebugUI::ProcessEvent(const void* sdlEvent)
{
    if (m_Initialized && sdlEvent != nullptr)
    {
        ImGui_ImplSDL3_ProcessEvent(static_cast<const SDL_Event*>(sdlEvent));
    }
}

void buki::DebugUI::BeginFrame()
{
    if (!m_Initialized)
    {
        return;
    }

    IInput& input = Engine::Get().Input();

    if (input.IsRawKeyDown(EKey::EKEY_F1))
    {
        m_Visible = !m_Visible;
    }

    if (input.IsRawKeyDown(EKey::EKEY_F2))
    {
        m_InputBlocked = !m_InputBlocked;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();


    if (m_Visible)
    {
        DrawMenuBar();
    }

    DrawDockspace();
    DrawWindows();
    DrawBlockedIndicator();

    const ImGuiIO& io = ImGui::GetIO();

    input.SetGameplayInputBlocked(m_InputBlocked || io.WantCaptureMouse, m_InputBlocked || io.WantCaptureKeyboard);
}

void buki::DebugUI::DrawDockspace()
{
    const ImGuiDockNodeFlags flags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingOverCentralNode;
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), flags);
}

void buki::DebugUI::Render()
{
    if (!m_Initialized)
    {
        return;
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    const ImGuiIO& io = ImGui::GetIO();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        SDL_Window* backupWindow = SDL_GL_GetCurrentWindow();
        SDL_GLContext backupContext = SDL_GL_GetCurrentContext();

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        SDL_GL_MakeCurrent(backupWindow, backupContext);
    }
}

bool buki::DebugUI::RegisterWindow(const std::string& id, DebugMenuGroup group, const std::string& title, DrawCallback callback, const void* owner, bool openByDefault)
{
    if (id.empty() || !callback)
    {
        return false;
    }

    for (Entry& entry : m_Windows)
    {
        if (entry.id == id)
        {
            entry.title = title;
            entry.group = group;
            entry.callback = std::move(callback);
            entry.owner = owner;
            return true;
        }
    }

    Entry entry;
    entry.id = id;
    entry.title = title;
    entry.group = group;
    entry.callback = std::move(callback);
    entry.owner = owner;
    entry.open = openByDefault;

    m_Windows.push_back(std::move(entry));
    return true;
}

void buki::DebugUI::UnregisterWindow(const std::string& id)
{
    m_Windows.erase(std::remove_if(m_Windows.begin(), m_Windows.end(), [&id](const Entry& entry) { return entry.id == id; }), m_Windows.end());
}

void buki::DebugUI::UnregisterOwner(const void* owner)
{
    if (owner == nullptr)
    {
        return;
    }

    m_Windows.erase(std::remove_if(m_Windows.begin(), m_Windows.end(), [owner](const Entry& entry) { return entry.owner == owner; }), m_Windows.end());
}

void buki::DebugUI::DrawMenuBar()
{
    if (!ImGui::BeginMainMenuBar())
    {
        return;
    }

    const auto drawGroup = [this](const char* label, DebugMenuGroup group)
        {
            if (!ImGui::BeginMenu(label))
            {
                return;
            }

            bool any = false;

            for (Entry& entry : m_Windows)
            {
                if (entry.group != group)
                {
                    continue;
                }

                any = true;
                ImGui::MenuItem(entry.title.c_str(), nullptr, &entry.open);
            }

            if (!any)
            {
                ImGui::TextDisabled("No windows registered");
            }

            ImGui::EndMenu();
        };

    drawGroup("Engine", DebugMenuGroup::Engine);
    drawGroup("Game", DebugMenuGroup::Game);

    ImGui::EndMainMenuBar();
}

void buki::DebugUI::DrawWindows()
{
    // Index-based so a callback may register windows without invalidating the loop.
    for (size_t i = 0; i < m_Windows.size(); ++i)
    {
        if (!m_Windows[i].open)
        {
            continue;
        }

        const std::string windowName = m_Windows[i].title + "###" + m_Windows[i].id;
        const DrawCallback callback = m_Windows[i].callback;

        bool open = true;

        ImGui::SetNextWindowSize(ImVec2(520.0f, 420.0f), ImGuiCond_FirstUseEver);

        if (ImGui::Begin(windowName.c_str(), &open))
        {
            callback();
        }

        ImGui::End();

        if (i < m_Windows.size())
        {
            m_Windows[i].open = open;
        }
    }
}

void buki::DebugUI::DrawBlockedIndicator()
{
    if (!m_InputBlocked)
    {
        return;
    }

    ImGuiViewport* viewport = ImGui::GetMainViewport();

    const char* text = "Game input blocked (F2)";
    const ImVec2 textSize = ImGui::CalcTextSize(text);
    const ImVec2 padding(8.0f, 4.0f);
    const float margin = 10.0f;

    const ImVec2 boxMax(viewport->WorkPos.x + viewport->WorkSize.x - margin, viewport->WorkPos.y + margin + textSize.y + padding.y * 2.0f);
    const ImVec2 boxMin(boxMax.x - textSize.x - padding.x * 2.0f, viewport->WorkPos.y + margin);

    ImDrawList* drawList = ImGui::GetForegroundDrawList(viewport);
    drawList->AddRectFilled(boxMin, boxMax, IM_COL32(20, 20, 20, 200), 4.0f);
    drawList->AddText(ImVec2(boxMin.x + padding.x, boxMin.y + padding.y), IM_COL32(255, 200, 50, 255), text);
}

#endif