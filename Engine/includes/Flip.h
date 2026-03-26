#pragma once

struct Flip
{
	Flip() : Flip(false, false) {}
	Flip(bool _v, bool _h) : v(_v), h(_h) {}
	bool v;
	bool h;
};