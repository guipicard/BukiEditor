#pragma once
#include "Entity.h"

namespace buki
{
	struct Prototypes
	{
		virtual Entity* Clone(Vector2 position, Vector2 size, float rotation) = 0;
		//virtual json Serialize() = 0;
	protected:
		int number = 0;
	};
}