#pragma once
#include "BukiContainers.h"

namespace buki
{
	struct Collider
	{
		virtual ~Collider() = default;
		bool m_CanDraw = false;
		Vector2 PositionOffset = { 0.0f, 0.0f };
		float Density = 1.0f;
		float Friction = 0.3f;
		float Restitution = 0.5f;
		//float RestitutionThreshold = 0.9f;
	};
}
