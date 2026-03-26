#pragma once

namespace buki
{
	struct MassData
	{
		MassData() : MassData(1.0f, 1.0f, 0.0f, 0.0f) {}
		MassData(float _mass, float _inv_mass, float _inertia, float _inv_inertia) : mass(_mass), inv_mass(_inv_mass), inertia(_inertia), inverse_inertia(_inv_inertia) {}
		float mass;
		float inv_mass;
		float inertia;
		float inverse_inertia;
	};
}
