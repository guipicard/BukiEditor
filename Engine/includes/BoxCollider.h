#pragma once
#include "Collider.h"

namespace buki
{
	
	struct BoxCollider : public Collider
	{
		BoxCollider() = default;
		~BoxCollider() = default;

		Vector2 Size = { 1.0f, 1.0f };
	};
}
