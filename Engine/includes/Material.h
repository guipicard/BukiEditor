#pragma once

namespace buki
{
	struct Material
	{
		Material() : Material(0.0f, 0.0f) {}
		Material(float _density, float _restitution) : density(_density), restitution(_restitution) {}
		float density;
		float restitution;
		
		static const Material& Rock;
		static const Material& Wood;
		static const Material& Metal;
		static const Material& SuperBall;
		static const Material& Pillow;
		static const Material& Static;
	};
}
