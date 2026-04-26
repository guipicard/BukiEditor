#include "Editor.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include <cstdio>
#include <SDL3/SDL_opengl.h>

#ifdef _WIN32
#include <windows.h>
#endif

buki::Editor::Editor() = default;
buki::Editor::~Editor() = default;

bool buki::Editor::Init()
{
#ifdef _WIN32
	::SetProcessDPIAware();
#endif

#if defined(IMGUI_IMPL_OPENGL_ES2)
	const char* glsl_version = "#version 100";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
	const char* glsl_version = "#version 300 es";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
	const char* glsl_version = "#version 150";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	main_scale = 1.0f;

	gameWindow = SDL_CreateWindow(
		"Buki Editor",
		1920,
		1080,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
	);

	if (!gameWindow)
	{
		std::printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return false;
	}

	gl_context = SDL_GL_CreateContext(gameWindow);
	if (!gl_context)
	{
		std::printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
		return false;
	}

	SDL_GL_MakeCurrent(gameWindow, gl_context);
	SDL_GL_SetSwapInterval(1);

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

	if (!ImGui_ImplSDL3_InitForOpenGL(gameWindow, gl_context))
	{
		std::printf("Error: ImGui_ImplSDL3_InitForOpenGL() failed\n");
		return false;
	}

	if (!ImGui_ImplOpenGL3_Init(glsl_version))
	{
		std::printf("Error: ImGui_ImplOpenGL3_Init() failed\n");
		return false;
	}

	return true;
}

void buki::Editor::Update(SDL_Event const& e)
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
		SDL_GLContext backup_context = SDL_GL_GetCurrentContext();

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		SDL_GL_MakeCurrent(backup_window, backup_context);
	}
}

void buki::Editor::Present()
{
	SDL_GL_SwapWindow(gameWindow);
}

void buki::Editor::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	if (gl_context)
	{
		SDL_GL_DestroyContext(gl_context);
		gl_context = nullptr;
	}

	if (gameWindow)
	{
		SDL_DestroyWindow(gameWindow);
		gameWindow = nullptr;
	}
}