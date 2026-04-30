#pragma once

#include "EKey.h"

namespace buki
{
    class IInput
    {
    public:
        virtual ~IInput() = default;

        virtual void BeginFrame() = 0;
        virtual void ProcessEvent(const void* event) = 0;
        virtual void EndFrame() = 0;
        virtual void ConsumeTransientInputs() = 0;

        virtual bool IsKeyPressed(EKey key) const = 0;
        virtual bool IsKeyDown(EKey key) const = 0;
        virtual bool IsKeyUp(EKey key) const = 0;

        virtual bool IsMouseButtonPressed(int button) const = 0;
        virtual bool IsMouseButtonDown(int button) const = 0;
        virtual bool IsMouseButtonUp(int button) const = 0;

        virtual void GetMousePositionScreen(int* x, int* y) const = 0;
        virtual void GetMousePositionWorld(float* x, float* y) const = 0;
        virtual int GetMouseWheelDelta() const = 0;
    };
}
