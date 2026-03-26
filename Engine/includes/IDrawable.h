#pragma once

namespace buki
{
	struct IDrawable
	{
		virtual ~IDrawable() = default;
		virtual void Draw(float alpha) = 0;
	};
}