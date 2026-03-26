#pragma once
#include "Collider.h"

namespace buki
{
	struct CircleCollider : public Collider
	{
		CircleCollider() = default;
		~CircleCollider() = default;

		float Radius = 1.0f;
	};
}
