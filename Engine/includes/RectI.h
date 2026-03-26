#pragma once

namespace buki
{

	struct RectI
	{
		RectI() : RectI(0, 0, 0, 0) {}
		RectI(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}

		/*bool operator==(const RectI& c)
		{
			return x == c.x && y == c.y && w == c.w && h == c.h;
		}*/
		int x;
		int y;
		int w;
		int h;
	};
}
