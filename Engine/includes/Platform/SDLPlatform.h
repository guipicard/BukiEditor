#pragma once

#include "Platform/IPlatform.h"
#include <SDL3/SDL.h>
#include <cstdint>

namespace buki
{
	class IInput;

	class SDLPlatform final : public IPlatform
	{
	public:
		SDLPlatform() = default;
		~SDLPlatform() override;

		bool Initialize(const PlatformWindowDesc& desc) override;
		void Shutdown() override;

		void SetInput(IInput* input);

		void PumpEvents() override;
		bool IsRunning() const override;
		void RequestQuit() override;

		void* GetNativeWindowHandle() const override;
		void* GetGraphicsContextHandle() const override;
		void* GetProcAddress(const char* procName) const override;

		std::uint32_t GetWindowID() const override;
		int GetWindowWidth() const override;
		int GetWindowHeight() const override;
		int GetDrawableWidth() const override;
		int GetDrawableHeight() const override;

		void SetVSync(bool enabled) override;
		bool IsVSyncEnabled() const override;
		void SwapBuffers() override;

		void* GetWindow() const;
		void* GetGLContext() const;

	private:
		bool InitializeSDL();
		bool CreateSDLWindow(const PlatformWindowDesc& desc);
		bool CreateOpenGLContext();
		void UpdateWindowSizeCache();
		void ResetState();

	private:
		SDL_Window* m_Window = nullptr;
		void* m_GLContext = nullptr;
		IInput* m_Input = nullptr;

		bool m_IsRunning = false;
		bool m_VSync = true;

		int m_WindowWidth = 0;
		int m_WindowHeight = 0;
		int m_DrawableWidth = 0;
		int m_DrawableHeight = 0;
	};
}