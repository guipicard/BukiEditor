#pragma once

#include "IInput.h"

#include <array>

namespace buki
{
    class SDLInput final : public IInput
    {
    public:
        void BeginFrame() override;
        void ProcessEvent(const void* event) override;
        void EndFrame() override;
        void ConsumeTransientInputs() override;

        bool IsKeyPressed(EKey key) const override;
        bool IsKeyDown(EKey key) const override;
        bool IsKeyUp(EKey key) const override;

        bool IsMouseButtonPressed(int button) const override;
        bool IsMouseButtonDown(int button) const override;
        bool IsMouseButtonUp(int button) const override;

        void GetMousePositionScreen(int* x, int* y) const override;
        int GetMouseWheelDelta() const override;

    private:
        static constexpr size_t KeyCount = 512;
        static constexpr size_t MouseButtonCount = 8;

    private:
        std::array<bool, KeyCount> m_CurrentKeys{};
        std::array<bool, KeyCount> m_PreviousKeys{};
        std::array<bool, KeyCount> m_KeyDown{};
        std::array<bool, KeyCount> m_KeyUp{};

        std::array<bool, MouseButtonCount> m_CurrentMouseButtons{};
        std::array<bool, MouseButtonCount> m_PreviousMouseButtons{};
        std::array<bool, MouseButtonCount> m_MouseButtonDown{};
        std::array<bool, MouseButtonCount> m_MouseButtonUp{};

        int m_MouseX = 0;
        int m_MouseY = 0;
        int m_MouseWheelDelta = 0;
    };
}
