#pragma once
#include "BukiContainers.h"

namespace buki
{
	struct SurfaceMaterial
	{
		/// The Coulomb (dry) friction coefficient, usually in the range [0,1].
		float friction = 0.3f;

		/// The coefficient of restitution (bounce) usually in the range [0,1].
		/// https://en.wikipedia.org/wiki/Coefficient_of_restitution
		float restitution = 0.5f;

		/// The rolling resistance usually in the range [0,1].
		float rollingResistance = 0.3f;

		/// The tangent speed for conveyor belts
		float tangentSpeed;

		/// User material identifier. This is passed with query results and to friction and restitution
		/// combining functions. It is not used internally.
		int userMaterialId;
	};

	struct Collider
	{
		virtual ~Collider() = default;
		bool m_CanDraw = false;
		SurfaceMaterial material = SurfaceMaterial();
		Vector2 PositionOffset = { 0.0f, 0.0f };
		float density = 1.0f;
		//float RestitutionThreshold = 0.9f;
	};
}
