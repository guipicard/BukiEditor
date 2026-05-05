#pragma once

#include "EditorState.h"

namespace buki
{
	namespace EditorViewportFramebuffer
	{
		bool Initialize(EditorState& state);
		void Shutdown(EditorState& state);
		bool Resize(EditorState& state, int width, int height);
		void Bind(const EditorState& state);
		void Unbind();
	}
}