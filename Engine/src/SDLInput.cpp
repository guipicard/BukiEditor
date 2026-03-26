#pragma once
#include "SDLInput.h"
#include "Engine.h"
#include <SDL.h>
#include "IScene.h"
#include "Camera.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"

using namespace buki;

void SdlInput::Update()
{
	SDL_Event _event;
	Uint32 windowID = buki::Engine::GetInstance().Graphics().GetWindowID();
	while (SDL_PollEvent(&_event)) {
		ImGui_ImplSDL2_ProcessEvent(&_event);
		switch (_event.type) {
		case SDL_WINDOWEVENT: {
			if (_event.window.windowID == windowID) {
				switch (_event.window.event) {

				case SDL_WINDOWEVENT_SIZE_CHANGED: {
					IScene* scene = buki::Engine::GetInstance().World().GetCurrentScene();
					if (scene)
					{
						scene->OnWindowResize();
					}
					break;
				}

				}
			}
			break;
		}
		case SDL_QUIT:
			m_IsRunning = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			SDL_MouseButtonEvent _buttonDown = _event.button;
			m_MouseStates[_buttonDown.button - 1] = true;
			break;
		case SDL_MOUSEBUTTONUP:
			SDL_MouseButtonEvent _buttonUp = _event.button;
			m_MouseStates[_buttonUp.button - 1] = false;
			break;
		case SDL_MOUSEMOTION:
			SDL_MouseMotionEvent _motion = _event.motion;
			m_MouseX = _motion.x;
			m_MouseY = _motion.y;
			break;
		case SDL_MOUSEWHEEL:
			SDL_MouseWheelEvent _wheel = _event.wheel;
			mouseWheel = _wheel.y; // Store the mouse wheel scroll value
			// Handle mouse wheel event
			break;
		}
	}
	m_KeyStates = SDL_GetKeyboardState(nullptr);
#if _DEBUG
	if (IsKeyDown(EKey::EKEY_ESCAPE))
	{
		ExitProgram();
	}
#endif
}

bool buki::SdlInput::IsKeyPressed(EKey keycode)
{
	return m_KeyStates[static_cast<int>(keycode)] == 1;
}

bool SdlInput::IsKeyDown(EKey keycode)
{
	return m_KeyStates[static_cast<int>(keycode)] == 1 && !m_LateKeyStates[static_cast<int>(keycode)];
}

bool buki::SdlInput::IsKeyUp(EKey keycode)
{
	return m_KeyStates[static_cast<int>(keycode)] != 1 && m_LateKeyStates[static_cast<int>(keycode)];
}


bool SdlInput::IsButtonPressed(int button)
{
	return m_MouseStates[button];
}

bool buki::SdlInput::IsButtonDown(int button)
{
	return m_MouseStates[button] && !m_LateMouseStates[button];
}

bool buki::SdlInput::IsButtonUp(int button)
{
	return !m_MouseStates[button] && m_LateMouseStates[button];
}

void SdlInput::GetMousePosition(float* x, float* y)
{
	float scale;
	buki::Engine::GetInstance().Graphics().GetScale(&scale);
	*x = (float)m_MouseX / scale;
	*y = (float)m_MouseY / scale;
	Camera* camera = buki::Engine::GetInstance().Graphics().GetCamera();
	Vector2 camPos = camera->position;
	Vector2 camSize = Vector2(camera->width / scale, camera->height / scale);
	*x += camPos.x - (camSize.x * 0.5f);
	*y += camPos.y - (camSize.y * 0.5f);
}

void buki::SdlInput::GetMousePositionScreen(int* x, int* y)
{
	*x = m_MouseX;
	*y = m_MouseY;
}

bool buki::SdlInput::GetMouseScrollUp(int* amount)
{
	if (mouseWheel > 0)
	{
		*amount = mouseWheel;
		return true;
	}
	return false;
}

bool buki::SdlInput::GetMouseScrollDown(int* amount)
{
	if (mouseWheel < 0)
	{
		*amount = -mouseWheel; // Make it positive for consistency
		return true;
	}
	return false;
}

void buki::SdlInput::ResetLateInputs()
{
	if (m_KeyStates != nullptr)
	{
		for (int i = 0; i < sizeof(m_LateKeyStates) / sizeof(m_LateKeyStates[0]); i++)
		{
			m_LateKeyStates[i] = m_KeyStates[i] == 1 ? true : false;
		}
	}
	for (int i = 0; i < sizeof(m_LateMouseStates) / sizeof(m_LateMouseStates[0]); i++)
	{
		m_LateMouseStates[i] = m_MouseStates[i] ? true : false;
	}
	mouseWheel = 0; // Reset mouse wheel scroll value
}

void buki::SdlInput::ExitProgram()
{
	SDL_Event quitEvent;
	quitEvent.type = SDL_QUIT;
	SDL_PushEvent(&quitEvent);
}
