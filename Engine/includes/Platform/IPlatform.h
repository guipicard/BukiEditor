#pragma once

#include <cstdint>
#include <string>

namespace buki
{
    struct IInput;

    struct PlatformWindowDesc
    {
        std::string title = "Buki Engine";
        int width = 1280;
        int height = 720;
        bool resizable = true;
        bool highDpi = true;
        bool startMaximized = false;
        bool vsync = true;
    };

    class IPlatform
    {
    public:
        virtual ~IPlatform() = default;

        virtual bool Initialize(const PlatformWindowDesc& desc) = 0;
        virtual void Shutdown() = 0;

        virtual void SetInput(IInput* input) = 0;

        virtual void PumpEvents() = 0;
        virtual bool IsRunning() const = 0;
        virtual void RequestQuit() = 0;

        virtual void* GetNativeWindowHandle() const = 0;
        virtual void* GetGraphicsContextHandle() const = 0;

        virtual void* GetProcAddress(const char* procName) const = 0;

        virtual std::uint32_t GetWindowID() const = 0;

        virtual int GetWindowWidth() const = 0;
        virtual int GetWindowHeight() const = 0;
        virtual int GetDrawableWidth() const = 0;
        virtual int GetDrawableHeight() const = 0;

        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSyncEnabled() const = 0;

        virtual void SwapBuffers() = 0;
    };
}
