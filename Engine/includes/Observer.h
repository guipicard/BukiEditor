#pragma once

namespace buki
{
	template<typename T1>
	struct Observer
	{
		virtual ~Observer() = default;
		virtual void OnNotify(const T1& value) = 0;
	};
}
