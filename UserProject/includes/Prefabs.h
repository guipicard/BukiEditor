#pragma once
#include "Prototypes.h"

namespace buki
{
	struct CircleShape : public Prototypes
	{
		CircleShape() {}
		virtual ~CircleShape() = default;
		virtual Entity* Clone(Vector2 position, Vector2 size, float rotation) override;

	};
	struct RectangleShape : public Prototypes
	{
		RectangleShape() {}
		virtual ~RectangleShape() = default;
		virtual Entity* Clone(Vector2 position, Vector2 size, float rotation) override;
	};
	struct StoneSlim : public Prototypes
	{
		StoneSlim() {};
		virtual ~StoneSlim() = default;
		virtual Entity* Clone(Vector2 position, Vector2 size, float rotation) override;
	};

	struct WoodSlim : public Prototypes
	{
		WoodSlim() {};
		virtual ~WoodSlim() = default;
		virtual Entity* Clone(Vector2 position, Vector2 size, float rotation) override;
	};

	struct GlassSlim : public Prototypes
	{
		GlassSlim() {};
		virtual ~GlassSlim() = default;
		virtual Entity* Clone(Vector2 position, Vector2 size, float rotation) override;
	};

	struct Bird : public Prototypes
	{
		Bird() {};
		virtual ~Bird() = default;
		virtual Entity* Clone(Vector2 position, Vector2 size, float rotation) override;
	};

	struct ButtonSimple : public Prototypes
	{
		ButtonSimple() {};
		virtual ~ButtonSimple() = default;
		virtual Entity* Clone(Vector2 position, Vector2 size, float rotation) override;
	};

	struct PauseScreen : public Prototypes
	{
		PauseScreen() {};
		virtual ~PauseScreen() = default;
		virtual Entity* Clone(Vector2 position, Vector2 size, float rotation) override;
	};
}