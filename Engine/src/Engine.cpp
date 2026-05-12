#include "Engine.h"

#include "Core/LogConsole.h"
#include "Core/LogFile.h"
#include "Platform/SDLPlatform.h"
#include "Graphics/OpenGLGraphics.h"
#include "Core/SDLInput.h"
#include "Core/WorldService.h"
#include "Core/SDL_Audio.h"
#include "PhysicsService.h"

using namespace buki;

bool Engine::Init(const std::string& title, int w, int h)
{
#if _DEBUG
	m_Console = MakeScope<LogConsole>();
#else
	m_Console = new LogFile("./assets/LogFile.txt");
#endif

	m_Console->LogSuccess("Logger initialized");

	m_Platform = MakeScope<SDLPlatform>();
	if (!m_Platform->Initialize(PlatformWindowDesc{ title, w, h }))
	{
		m_Console->LogSdlError();
		return false;
	}

	m_Graphics = MakeScope<OpenGLGraphics>();
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

	m_Input = MakeScope<SDLInput>();
	m_Platform->SetInput(m_Input.get());
	m_World = MakeScope<WorldService>();
	m_Audio = MakeScope<SDL_Audio>();
	m_Physics = MakeScope<PhysicsService>();

	m_Physics->InitPhysics();

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
	m_Audio->Destroy();
	m_Audio.reset();

	m_TextureManager->Clear();
	m_TextureManager.reset();

	m_FontManager->Clear();
	m_FontManager.reset();

	m_Physics->Destroy();
	m_Physics.reset();

	m_World->Destroy();
	m_World.reset();

	m_Graphics->Shutdown();
	m_Graphics.reset();

	m_Console.reset();

	m_Input.reset();

	m_Platform->Shutdown();
	m_Platform.reset();

	m_IsInit = false;
}
