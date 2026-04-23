#include "SDLInput.h"

#include <SDL3/SDL.h>
#include <algorithm>

namespace buki
{
    void SDLInput::BeginFrame()
    {
        m_PreviousKeys = m_CurrentKeys;
        m_PreviousMouseButtons = m_CurrentMouseButtons;

        m_KeyDown.fill(false);
        m_KeyUp.fill(false);
        m_MouseButtonDown.fill(false);
        m_MouseButtonUp.fill(false);
        m_MouseWheelDelta = 0;
    }

    void SDLInput::ProcessEvent(const void* eventPtr)
    {
        if (eventPtr == nullptr)
        {
            return;
        }

        const SDL_Event& event = *static_cast<const SDL_Event*>(eventPtr);
        switch (event.type)
        {
        case SDL_EVENT_KEY_DOWN:
        {
            const SDL_Scancode scancode = event.key.scancode;
            const size_t index = static_cast<size_t>(scancode);
            if (index < KeyCount)
            {
                if (!m_CurrentKeys[index])
                {
                    m_KeyDown[index] = true;
                }

                m_CurrentKeys[index] = true;
            }
            break;
        }

        case SDL_EVENT_KEY_UP:
        {
            const SDL_Scancode scancode = event.key.scancode;
            const size_t index = static_cast<size_t>(scancode);

            if (index < KeyCount)
            {
                if (m_CurrentKeys[index])
                {
                    m_KeyUp[index] = true;
                }

                m_CurrentKeys[index] = false;
            }
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            const int buttonIndex = static_cast<int>(event.button.button) - 1;

            if (buttonIndex >= 0 && buttonIndex < static_cast<int>(MouseButtonCount))
            {
                if (!m_CurrentMouseButtons[buttonIndex])
                {
                    m_MouseButtonDown[buttonIndex] = true;
                }

                m_CurrentMouseButtons[buttonIndex] = true;
            }
            break;
        }

        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            const int buttonIndex = static_cast<int>(event.button.button) - 1;

            if (buttonIndex >= 0 && buttonIndex < static_cast<int>(MouseButtonCount))
            {
                if (m_CurrentMouseButtons[buttonIndex])
                {
                    m_MouseButtonUp[buttonIndex] = true;
                }

                m_CurrentMouseButtons[buttonIndex] = false;
            }
            break;
        }

        case SDL_EVENT_MOUSE_MOTION:
            m_MouseX = static_cast<int>(event.motion.x);
            m_MouseY = static_cast<int>(event.motion.y);
            break;

        case SDL_EVENT_MOUSE_WHEEL:
            m_MouseWheelDelta += static_cast<int>(event.wheel.y);
            break;

        default:
            break;
        }
    }

    void SDLInput::EndFrame()
    {
    }

    void SDLInput::ConsumeTransientInputs()
    {
        m_KeyDown.fill(false);
        m_KeyUp.fill(false);
        m_MouseButtonDown.fill(false);
        m_MouseButtonUp.fill(false);
        m_MouseWheelDelta = 0;
    }

    bool SDLInput::IsKeyPressed(EKey key) const
    {
        const size_t index = static_cast<size_t>(key);
        return index < KeyCount ? m_CurrentKeys[index] : false;
    }

    bool SDLInput::IsKeyDown(EKey key) const
    {
        const size_t index = static_cast<size_t>(key);
        return index < KeyCount ? m_KeyDown[index] : false;
    }

    bool SDLInput::IsKeyUp(EKey key) const
    {
        const size_t index = static_cast<size_t>(key);
        return index < KeyCount ? m_KeyUp[index] : false;
    }

    bool SDLInput::IsMouseButtonPressed(int button) const
    {
        return button >= 0 && button < static_cast<int>(MouseButtonCount)
            ? m_CurrentMouseButtons[button]
            : false;
    }

    bool SDLInput::IsMouseButtonDown(int button) const
    {
        return button >= 0 && button < static_cast<int>(MouseButtonCount)
            ? m_MouseButtonDown[button]
            : false;
    }

    bool SDLInput::IsMouseButtonUp(int button) const
    {
        return button >= 0 && button < static_cast<int>(MouseButtonCount)
            ? m_MouseButtonUp[button]
            : false;
    }

    void SDLInput::GetMousePositionScreen(int* x, int* y) const
    {
        if (x) *x = m_MouseX;
        if (y) *y = m_MouseY;
    }

    int SDLInput::GetMouseWheelDelta() const
    {
        return m_MouseWheelDelta;
    }
}
