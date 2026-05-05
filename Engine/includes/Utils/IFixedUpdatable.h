#pragma once

namespace buki 
{
	struct IFixedUpdatable
	{
		virtual void FixedUpdate(float dt) = 0;
	};
}