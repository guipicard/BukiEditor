#pragma once

#include "EditorState.h"
#include "EditorCanvasNavigation.h"
#include "ImGui.h"

namespace buki
{
	class SceneViewPanel
	{
	public:
		void Render(EditorState& state);
	private:
		EditorCanvasNavigation m_CameraNavigation;
	};
}
