#pragma once

#include <cstdint>
#include <functional>

namespace buki
{
	using EditorId = std::uint64_t;

	EditorId GenerateEditorId();

	constexpr EditorId InvalidEditorId = 0;
}