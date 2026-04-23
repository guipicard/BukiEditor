#include "SDLPlatform.h"
#include "IInput.h"

#include <SDL3/SDL.h>

namespace buki
{
    namespace
    {
        constexpr int kGLMajorVersion = 3;
        constexpr int kGLMinorVersion = 3;
    }

    SDLPlatform::~SDLPlatform()
    {
        Shutdown();
    }

    void SDLPlatform::SetInput(IInput* input)
    {
        m_Input = input;
    }

    void SDLPlatform::PumpEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (m_Input != nullptr)
            {
                m_Input->ProcessEvent(&event);
            }

            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                m_IsRunning = false;
                break;

            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                UpdateWindowSizeCache();
                break;

            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                if (m_Window != nullptr && event.window.windowID == SDL_GetWindowID(m_Window))
                {
                    m_IsRunning = false;
                }
                break;

            default:
                break;
            }
        }
#if _DEBUG
        if (m_Input->IsKeyDown(EKey::EKEY_ESCAPE))
        {
            m_IsRunning = false;
        }
#endif
    }

    bool SDLPlatform::Initialize(const PlatformWindowDesc& desc)
    {
        Shutdown();

        if (!InitializeSDL())
        {
            return false;
        }

        if (!CreateWindow(desc))
        {
            Shutdown();
            return false;
        }

        if (!CreateOpenGLContext())
        {
            Shutdown();
            return false;
        }

        m_VSync = desc.vsync;
        SetVSync(m_VSync);

        UpdateWindowSizeCache();
        m_IsRunning = true;
        return true;
    }

    void SDLPlatform::Shutdown()
    {
        if (m_GLContext != nullptr)
        {
            SDL_GL_DestroyContext(m_GLContext);
            m_GLContext = nullptr;
        }

        if (m_Window != nullptr)
        {
            SDL_DestroyWindow(m_Window);
            m_Window = nullptr;
        }

        SDL_Quit();
        ResetState();
        m_Input = nullptr;
    }

    bool SDLPlatform::IsRunning() const
    {
        return m_IsRunning;
    }

    void SDLPlatform::RequestQuit()
    {
        m_IsRunning = false;
    }

    void* SDLPlatform::GetNativeWindowHandle() const
    {
        return m_Window;
    }

    void* SDLPlatform::GetGraphicsContextHandle() const
    {
        return m_GLContext;
    }

    void* SDLPlatform::GetProcAddress(const char* procName) const
    {
        return SDL_GL_GetProcAddress(procName);
    }


    SDL_GLContext SDLPlatform::GetGLContext() const
    {
        return m_GLContext;
    }

    std::uint32_t SDLPlatform::GetWindowID() const
    {
        return m_Window != nullptr ? SDL_GetWindowID(m_Window) : 0;
    }

    int SDLPlatform::GetWindowWidth() const
    {
        return m_WindowWidth;
    }

    int SDLPlatform::GetWindowHeight() const
    {
        return m_WindowHeight;
    }

    int SDLPlatform::GetDrawableWidth() const
    {
        return m_DrawableWidth;
    }

    int SDLPlatform::GetDrawableHeight() const
    {
        return m_DrawableHeight;
    }

    void SDLPlatform::SetVSync(bool enabled)
    {
        m_VSync = enabled;

        if (m_GLContext == nullptr)
        {
            return;
        }

        SDL_GL_SetSwapInterval(enabled ? 1 : 0);
    }

    bool SDLPlatform::IsVSyncEnabled() const
    {
        return m_VSync;
    }

    void SDLPlatform::SwapBuffers()
    {
        if (m_Window != nullptr)
        {
            SDL_GL_SwapWindow(m_Window);
        }
    }

    bool SDLPlatform::InitializeSDL()
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
        {
            return false;
        }

        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, kGLMajorVersion))
        {
            return false;
        }

        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, kGLMinorVersion))
        {
            return false;
        }

        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE))
        {
            return false;
        }

        if (!SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1))
        {
            return false;
        }

        if (!SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24))
        {
            return false;
        }

        if (!SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8))
        {
            return false;
        }

        return true;
    }

    bool SDLPlatform::CreateWindow(const PlatformWindowDesc& desc)
    {
        SDL_WindowFlags flags = SDL_WINDOW_OPENGL;

        if (desc.resizable)       flags |= SDL_WINDOW_RESIZABLE;
        if (desc.highDpi)         flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
        if (desc.startMaximized)  flags |= SDL_WINDOW_MAXIMIZED;

        m_Window = SDL_CreateWindow(desc.title.c_str(), desc.width, desc.height, flags);
        return m_Window != nullptr;
    }

    bool SDLPlatform::CreateOpenGLContext()
    {
        if (m_Window == nullptr)
        {
            return false;
        }

        m_GLContext = SDL_GL_CreateContext(m_Window);
        if (m_GLContext == nullptr)
        {
            return false;
        }

        if (!SDL_GL_MakeCurrent(m_Window, m_GLContext))
        {
            return false;
        }

        return true;
    }

    void SDLPlatform::UpdateWindowSizeCache()
    {
        if (m_Window == nullptr)
        {
            m_WindowWidth = 0;
            m_WindowHeight = 0;
            m_DrawableWidth = 0;
            m_DrawableHeight = 0;
            return;
        }

        SDL_GetWindowSize(m_Window, &m_WindowWidth, &m_WindowHeight);
        SDL_GetWindowSizeInPixels(m_Window, &m_DrawableWidth, &m_DrawableHeight);
    }

    void SDLPlatform::ResetState()
    {
        m_IsRunning = false;
        m_VSync = true;
        m_WindowWidth = 0;
        m_WindowHeight = 0;
        m_DrawableWidth = 0;
        m_DrawableHeight = 0;
    }
}
