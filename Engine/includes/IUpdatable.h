#pragma once

namespace buki 
{
	struct IUpdatable
	{
		virtual void Update(float dt) = 0;
	};
}