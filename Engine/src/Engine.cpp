#pragma once
#include "Engine.h"
#include "Editor.h"
#include "SDLInput.h"
#include "LogConsole.h"
#include "LogFile.h"
#include "SDL_Graphics.h"
#include "WorldService.h"
#include "BaseScene.h"
#include "SDL_Audio.h"
#include "SDL_timer.h"
#include "PhysicsService.h"
#include "box2d.h"
#include <algorithm>
#include <cstdint>
//#include <vld.h>

using namespace buki;

const Vector2& Vector2::ZERO = Vector2(0.0f, 0.0f);
const Vector2& Vector2::RIGHT = Vector2(1.0f, 0.0f);
const Vector2& Vector2::UP = Vector2(0.0f, -1.0f);
const Vector2& Vector2::LEFT = Vector2(-1.0f, 0.0f);
const Vector2& Vector2::DOWN = Vector2(0.0f, 1.0f);

bool Engine::Init(const std::string& name, int w, int h)
{
#if _DEBUG
	m_Console = new LogConsole();
#else
	m_Console = new LogFile();
#endif
	m_Console->LogSuccess("SDL initialised");
	m_Graphics = new SDL_Graphics();
	if (!m_Graphics)
	{
		m_Console->LogSdlError();
		return false;
	}
	m_Graphics->InitBackend();
#if BUKI_EDITOR
	m_Editor = new Editor();
	m_Editor->Init();
#else
	m_Editor = nullptr;
#endif

	if (!m_Graphics->Initialize(name, w, h))
	{
		return false;
	}
	m_Console->LogSuccess("Graphics initialised");
	m_Input = new SdlInput();
	m_World = new WorldService();
	m_Audio = new SDL_Audio();
	m_Physics = new PhysicsService();

	m_IsInit = true;
	return m_IsInit;
}

void Engine::Start(void) {
	if (!m_IsInit) {
		if (!Init("Buki Engine", 1920, 1080)) {
			return;
		}
	}
	m_Console->LogSuccess("Buki initialized");
	Input().m_IsRunning = true;

	Uint32 lastTime = SDL_GetTicks();
	Uint32 lag = 0;
	Uint32 fixedLag = 0;

	// Optional: For FPS calculation
	Uint32 frameCount = 0;
	Uint32 physicsCount = 0;
	Uint32 fpsLastTime = lastTime;

	while (Input().m_IsRunning) {
		Uint32 currentTime = SDL_GetTicks();
		Uint32 elapsed = currentTime - lastTime;
		lastTime = currentTime;

		float dt = (float)(elapsed) * 0.001f;
		lag += elapsed;
		fixedLag += elapsed;


		ProcessInput();
		while (lag >= MS_PER_FRAME) {
			Update(dt * timeScale);
			Input().ResetLateInputs();
			lag -= MS_PER_FRAME;
		}

		while (fixedLag >= FIXED_TIMESTEP) {
			FixedUpdate(FIXED_TIMESTEP * 0.001f * timeScale);
			fixedLag -= FIXED_TIMESTEP;
			physicsCount++;
		}

		float alpha = (float)(fixedLag / FIXED_TIMESTEP);
		//float alpha = static_cast<float>(lag / MS_PER_FRAME);
		alpha = alpha < 0.0f ? 0.0f : alpha > 1.0f ? 1.0f : alpha;
		alpha = 1.0f;
		if (m_Editor != nullptr)
		{
			m_Editor->EditorClear();
			m_Editor->Render();
			m_Editor->EditorPresent();
			Render(alpha);
		}
		else
		{
			Render(alpha);
		}



		frameCount++;
		if (currentTime - fpsLastTime >= 1000) {
			m_CurrentFPS = frameCount;
			frameCount = 0;
			fpsLastTime = currentTime;

			m_CurrentPPS = physicsCount;
			physicsCount = 0;
		}

		Uint32 frameTime = SDL_GetTicks() - currentTime;
		if (MS_PER_FRAME > frameTime) {
			SDL_Delay(MS_PER_FRAME - frameTime);
		}
	}

	Shutdown();
}

void buki::Engine::SetTimeScale(const float scale)
{
	timeScale = scale;
}

void Engine::ProcessInput(void) const
{
	Input().Update();
}

void buki::Engine::FixedUpdate(float dt)
{
	m_World->FixedUpdate(dt);
}

void Engine::Update(float dt)
{
	m_World->Update(dt);
}

void Engine::Render(float alpha)
{
	m_Graphics->Clear();
	m_World->Render(alpha);
	m_Graphics->Present();

}

void Engine::Shutdown(void)
{
	if (m_Editor != nullptr)
	{
		m_Editor->Shutdown();
		delete m_Editor;
	}
	if (m_Input != nullptr)
	{
		delete m_Input;
	}
	if (m_Console != nullptr)
	{
		delete m_Console;
	}
	if (m_Graphics != nullptr)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
	}
	if (m_Audio != nullptr)
	{
		m_Audio->Destroy();
		delete m_Audio;
	}
	if (m_World != nullptr)
	{
		m_World->Destroy();
		delete m_World;
	}
	if (m_Physics != nullptr)
	{
		m_Physics->Destroy();
		delete m_Physics;
	}
}
