#pragma once

#include "EditorState.h"

namespace buki
{
	class InspectorPanel
	{
	public:
		void Render(EditorState& state);

	private:
		bool DrawStringField(const char* label, std::string& value);
	};
}