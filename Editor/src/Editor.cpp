#include "Editor.h"

#include "Engine.h"
#include "Platform/SDLPlatform.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include <SDL3/SDL_opengl.h>
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#endif

#include "EditorViewportFramebuffer.h"

buki::Editor::Editor() = default;
buki::Editor::~Editor() = default;

bool buki::Editor::Init()
{
#ifdef _WIN32
	::SetProcessDPIAware();
#endif

	if (!buki::Engine::Get().Init("Buki Editor", 1920, 1080))
	{
		return false;
	}

	auto* platform = dynamic_cast<buki::SDLPlatform*>(&buki::Engine::Get().Platform());
	if (platform == nullptr)
	{
		std::printf("Error: Engine platform is not SDLPlatform\n");
		return false;
	}

	gameWindow = (SDL_Window*)platform->GetWindow();
	void* glContext = platform->GetGLContext();

	if (gameWindow == nullptr || glContext == nullptr)
	{
		std::printf("Error: SDL window or GL context is null\n");
		return false;
	}

	const char* glsl_version = "#version 130";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	if (!ImGui_ImplSDL3_InitForOpenGL(gameWindow, glContext))
	{
		std::printf("Error: ImGui_ImplSDL3_InitForOpenGL() failed\n");
		buki::Engine::Get().Platform().RequestQuit();
		return false;
	}

	if (!ImGui_ImplOpenGL3_Init(glsl_version))
	{
		std::printf("Error: ImGui_ImplOpenGL3_Init() failed\n");
		return false;
	}

	return true;
}

void buki::Editor::Update(const SDL_Event& e)
{
	ImGui_ImplSDL3_ProcessEvent(&e);
}

void buki::Editor::BeginFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

void buki::Editor::Render()
{
	ImGuiIO& io = ImGui::GetIO();
	
	editorLayer.Render();

	ImGui::Render();

	glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(
		clearColor[0] * clearColor[3],
		clearColor[1] * clearColor[3],
		clearColor[2] * clearColor[3],
		clearColor[3]
	);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_window = SDL_GL_GetCurrentWindow();
		void* backup_context = SDL_GL_GetCurrentContext();

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		if (backup_window != nullptr && backup_context != nullptr)
		{
			SDL_GL_MakeCurrent(backup_window, static_cast<SDL_GLContext>(backup_context));
		}
	}
}

void buki::Editor::Present()
{
	if (gameWindow != nullptr)
	{
		SDL_GL_SwapWindow(gameWindow);
	}
}

void buki::Editor::Shutdown()
{
	EditorViewportFramebuffer::Shutdown(editorLayer.State());
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
	buki::Engine::Get().Shutdown();
}
