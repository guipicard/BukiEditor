#pragma once
#include "IInput.h"

namespace buki {

	struct SdlInput final : public IInput
	{
	public:
		virtual ~SdlInput() = default;
		virtual void Update() override;
		virtual bool IsKeyPressed(EKey keycode) override;
		virtual bool IsKeyDown(EKey keycode) override;
		virtual bool IsKeyUp(EKey keycode) override;
		virtual bool IsButtonPressed(int button) override;
		virtual bool IsButtonDown(int button) override;
		virtual bool IsButtonUp(int button) override;
		virtual void GetMousePosition(float* x, float* y) override;
		virtual void GetMousePositionScreen(int* x, int* y) override;
		virtual bool GetMouseScrollUp(int* amount) override;
		virtual bool GetMouseScrollDown(int* amount) override;
		virtual void ResetLateInputs() override;
		virtual void ExitProgram() override;
	private:
		const unsigned char* m_KeyStates = nullptr;
		bool m_MouseStates[3]{ false };
		bool m_LateKeyStates[322]{ false };
		bool m_LateMouseStates[3]{ false };
		int m_MouseX = 0;
		int m_MouseY = 0;
		int mouseWheel = 0;
	};
}
