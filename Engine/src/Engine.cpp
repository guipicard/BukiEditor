#include "Engine.h"

#include "Platform/SDLPlatform.h"
#include "Graphics/OpenGLGraphics.h"
#include "Core/LogConsole.h"
#include "Core/LogFile.h"
#include "Core/WorldService.h"
#include "Core/SDLInput.h"
#include "PhysicsService.h"
#include "Animation.h"
#include "Entity.h"

#include "Graphics/IGraphics.h"

#include <array>

#include "Core/SDL_Audio.h"

//#include "vld.h"

using namespace buki;

bool Engine::Init(const std::string& title, int w, int h)
{
#if _DEBUG
	m_Console = new LogConsole();
#else
	m_Console = new LogFile("./assets/LogFile.txt");
#endif

	m_Console->LogSuccess("Logger initialized");

	m_Platform = new SDLPlatform();
	if (!m_Platform->Initialize(PlatformWindowDesc{ title, w, h }))
	{
		m_Console->LogSdlError();
		return false;
	}

	m_Graphics = new OpenGLGraphics();
	if (!m_Graphics->Initialize(*m_Platform))
	{
		m_Console->LogError("Graphics initialization failed");
		return false;
	}

	m_Camera = Camera2D();
	m_Camera.viewportWidth = static_cast<float>(w);
	m_Camera.viewportHeight = static_cast<float>(h);

	m_TextureManager = MakeScope<TextureManager>(*m_Graphics);
	m_FontManager = MakeScope<FontManager>(*m_Graphics);

	m_Input = new SDLInput();
	m_Platform->SetInput(m_Input);
	m_World = new WorldService();
	m_Audio = new SDL_Audio();
	m_Physics = new PhysicsService();

	m_IsInit = true;
	return true;
}

void Engine::Start()
{
	if (!m_IsInit)
	{
		if (!Init("Buki Engine", 1920, 1080))
		{
			return;
		}
	}

	m_Console->LogSuccess("Buki initialized");

	Uint64 lastTime = SDL_GetTicks();
	Uint64 accumulator = 0;
	Uint64 fixedAccumulator = 0;

	Uint32 frameCount = 0;
	Uint32 physicsCount = 0;
	Uint64 fpsLastTime = lastTime;
	while (Platform().IsRunning())
	{
		const Uint64 currentTime = SDL_GetTicks();
		const Uint64 elapsed = currentTime - lastTime;
		lastTime = currentTime;

		accumulator += elapsed;
		fixedAccumulator += elapsed;

		ProcessInput();
		bool consumedTransientInputs = false;

		while (accumulator >= MS_PER_FRAME)
		{
			Update(static_cast<float>(MS_PER_FRAME) * 0.001f * m_TimeScale);

			if (!consumedTransientInputs)
			{
				Input().ConsumeTransientInputs();
				consumedTransientInputs = true;
			}

			accumulator -= MS_PER_FRAME;
		}

		while (fixedAccumulator >= FIXED_TIMESTEP)
		{
			FixedUpdate(static_cast<float>(FIXED_TIMESTEP) * 0.001f * m_TimeScale);
			fixedAccumulator -= FIXED_TIMESTEP;
			++physicsCount;
		}

		const float alpha = static_cast<float>(accumulator) / static_cast<float>(MS_PER_FRAME);
		//Render(alpha < 0.0f ? 0.0f : alpha > 1.0f ? 1.0f : alpha);
		Render(alpha);

		++frameCount;

		if (currentTime - fpsLastTime >= 1000)
		{
			m_CurrentFPS = static_cast<int>(frameCount);
			m_CurrentPPS = static_cast<int>(physicsCount);
			frameCount = 0;
			physicsCount = 0;
			fpsLastTime = currentTime;
		}

		const Uint64 frameTime = SDL_GetTicks() - currentTime;
		if (frameTime < MS_PER_FRAME)
		{
			SDL_Delay(static_cast<Uint32>(MS_PER_FRAME - frameTime));
		}
	}

	Shutdown();
}

void Engine::SetTimeScale(float scale)
{
	m_TimeScale = scale;
}

void Engine::ProcessInput()
{
	Input().BeginFrame();
	Platform().PumpEvents();
	Input().EndFrame();

	if (m_Graphics != nullptr)
	{
		m_Graphics->Resize(Platform().GetDrawableWidth(), Platform().GetDrawableHeight());
	}
}

void Engine::FixedUpdate(const float dt)
{
	if (m_World != nullptr)
	{
		m_World->FixedUpdate(dt);
	}
}

void Engine::Update(float dt)
{
	if (m_World != nullptr)
	{
		m_World->Update(dt);
	}
}

void Engine::Render(float alpha)
{
	m_Graphics->BeginFrame();
	m_World->Render(alpha);
	m_Graphics->EndFrame();
}


void Engine::Shutdown()
{
	if (m_Audio != nullptr)
	{
	    m_Audio->Destroy();
	    delete m_Audio;
	    m_Audio = nullptr;
	}
	if (m_TextureManager)
	{
		m_TextureManager->Clear();
		m_TextureManager.reset();
	}
	if (m_FontManager)
	{
		m_FontManager->Clear();
		m_FontManager.reset();
	}
	if (m_World != nullptr)
	{
		m_World->Destroy();
		delete m_World;
		m_World = nullptr;
	}

	if (m_Physics != nullptr)
	{
		m_Physics->Destroy();
		delete m_Physics;
		m_Physics = nullptr;
	}

	if (m_Input != nullptr)
	{
		delete m_Input;
		m_Input = nullptr;
	}

	if (m_Graphics != nullptr)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}

	if (m_Platform != nullptr)
	{
		m_Platform->Shutdown();
		delete m_Platform;
		m_Platform = nullptr;
	}

	if (m_Console != nullptr)
	{
		delete m_Console;
		m_Console = nullptr;
	}

	SDL_Quit();
	m_IsInit = false;
}
