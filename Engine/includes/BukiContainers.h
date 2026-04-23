#pragma once
#include <float.h>
#include <cstdlib>
#include "math.h"

constexpr auto PI = 3.14159265359f;
static constexpr float METRES_TO_PIXELS = 50.0f;
static constexpr float DEG_TO_RAD = 0.0174533f; // PI/180
static constexpr float RAD_TO_DEG = 57.2958f;   // 180/PI

namespace buki
{
	/*struct RectF
	{
		RectF() : RectF(0.0f, 0.0f, 0.0f, 0.0f) {}
		RectF(int _x, int _y, int _w, int _h) : RectF(static_cast<float>(_x), static_cast<float>(_y), static_cast<float>(_w), static_cast<float>(_h)) {}
		RectF(float _x, float _y, float _w, float _h) : x(_x), y(_y), w(_w), h(_h) {}

		RectF operator+(RectF o) { return RectF(x + o.x, y + o.y, w + o.w, h + o.h); }
		RectF operator-(RectF o) { return RectF(x - o.x, y - o.y, w - o.w, h - o.h); }
		bool operator==(RectF o) { return x == o.x && y == o.y && w == o.w && h == o.h; }
		RectF operator*(float o) { return { x * o, y * o, w * o, h * o }; }

		float x;
		float y;
		float w;
		float h;
	};*/
	struct Color
	{
		float r = 1.0f;
		float g = 1.0f;
		float b = 1.0f;
		float a = 1.0f;
	};
	struct RectF
	{
		float x = 0.0f;
		float y = 0.0f;
		float w = 0.0f;
		float h = 0.0f;
	};
	struct UVRect
	{
		float u0, v0, u1, v1;
	};
	struct Vertex
	{
		float x;
		float y;
		float r;
		float g;
		float b;
		float a;
		float u;
		float v;
	};
	struct RectI
	{
		RectI() : RectI(0, 0, 0, 0) {}
		RectI(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}

		int x;
		int y;
		int w;
		int h;
	};

	struct Flip
	{
		Flip() : Flip(false, false) {}
		Flip(bool _v, bool _h) : v(_v), h(_h) {}
		bool v;
		bool h;
	};

	struct Rot
	{
		Rot() : Rot(1.0f, 0.0f) {}
		Rot(float _c, float _s) : c(_c), s(_s) {}
		float c, s;
		inline float GetRadians() const { return b2Atan2(s, c); }
		inline void SetRadians(float radians) 
		{
			float x = b2UnwindLargeAngle(radians);
			float pi2 = PI * PI;

			// cosine needs angle in [-pi/2, pi/2]
			float _c;
			if (x < -0.5f * PI)
			{
				float y = x + PI;
				float y2 = y * y;
				_c = -(pi2 - 4.0f * y2) / (pi2 + y2);
			}
			else if (x > 0.5f * PI)
			{
				float y = x - PI;
				float y2 = y * y;
				_c = -(pi2 - 4.0f * y2) / (pi2 + y2);
			}
			else
			{
				float y2 = x * x;
				_c = (pi2 - 4.0f * y2) / (pi2 + y2);
			}

			// sine needs angle in [0, pi]
			float _s;
			if (x < 0.0f)
			{
				float y = x + PI;
				_s = -16.0f * y * (PI - y) / (5.0f * pi2 - 4.0f * y * (PI - y));
			}
			else
			{
				_s = 16.0f * x * (PI - x) / (5.0f * pi2 - 4.0f * x * (PI - x));
			}

			float mag = sqrtf(_s * _s + _c * _c);
			float invMag = mag > 0.0 ? 1.0f / mag : 0.0f;
			c = _c * invMag;
			s = _s * invMag;
		}
			
		
	private:
		inline float b2UnwindLargeAngle(float radians) const
		{
			while (radians > PI)
			{
				radians -= 2.0f * PI;
			}

			while (radians < -PI)
			{
				radians += 2.0f * PI;
			}

			return radians;
		}
		inline float b2Atan2(float y, float x) const
		{
			// Added check for (0,0) to match atan2f and avoid NaN
			if (x == 0.0f && y == 0.0f)
			{
				return 0.0f;
			}

			float ax = x < 0 ? -x : x;
			float ay = y < 0 ? -y : y;
			float mx = ay > ax ? ay : ax;
			float mn = ay < ax ? ay : ax;
			float a = mn / mx;

			// Minimax polynomial approximation to atan(a) on [0,1]
			float s = a * a;
			float c = s * a;
			float q = s * s;
			float r = 0.024840285f * q + 0.18681418f;
			float t = -0.094097948f * q - 0.33213072f;
			r = r * s + t;
			r = r * c + a;

			// Map to full circle
			if (ay > ax)
			{
				r = 1.57079637f - r;
			}

			if (x < 0)
			{
				r = 3.14159274f - r;
			}

			if (y < 0)
			{
				r = -r;
			}

			return r;
		}

	};


	class Vector2
	{
	public:
		float x;
		float y;

		inline Vector2() : Vector2(0.0f, 0.0f) {}
		inline Vector2(const int& _x, const int& _y) : x(static_cast<float>(_x)), y(static_cast<float>(_y)) {};
		inline Vector2(const float& _x, const float& _y) : x(_x), y(_y) {};

		bool operator==(const Vector2& other) const { return x == other.x && y == other.y; }
		bool operator!=(const Vector2& other) const { return x != other.x || y != other.y; }
		inline Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
		inline Vector2& operator+=(const Vector2& other) { x += other.x, y += other.y; return *this; }
		inline Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
		inline Vector2 operator-() const { return Vector2(-x, -y); }
		inline Vector2& operator-=(const Vector2& other) { x = x - other.x, y = y - other.y; return *this; }
		inline Vector2 operator*(const float& other) const { return Vector2(x * other, y * other); }
		inline Vector2& operator*=(const float& other) { x *= other; y *= other; return *this; }
		inline Vector2 operator/(const float& other) const { return Vector2(x / other, y / other); }
		inline Vector2& operator/=(const float& other) { x /= other; y /= other; return *this; }
		inline Vector2& Abs() { x = x < 0 ? -x : x; y = y < 0 ? -y : y; return *this; }
		inline Vector2 GetAbs() const { return Vector2(x < 0 ? -x : x, y < 0 ? -y : y); }
		inline float Length() const { return sqrtf(x * x + y * y); }
		inline float LengthSquared() const { return x * x + y * y; }
		inline const Vector2 GetNormalized() {
			float length = sqrtf(x * x + y * y);
			if (length < FLT_EPSILON)
			{
				return ZERO;
			}

			float invLength = 1.0f / length;
			Vector2 n = { invLength * x, invLength * y };
			return n;
		}
		inline static const float Distance(const Vector2& a, const Vector2& b) {
			float dx = b.x - a.x;
			float dy = b.y - a.y;
			return sqrtf(dx * dx + dy * dy);
		}
		inline static const float DistanceSquared(const Vector2& a, const Vector2& b) {
			float dx = b.x - a.x;
			float dy = b.y - a.y;
			return dx * dx + dy * dy;
		}
		inline static const float Dot(const Vector2& A, const Vector2& B) { return (A.x * B.x) + (A.y * B.y); }
		inline static const float Cross(const Vector2& a, const Vector2& b) { return a.x * b.y - a.y * b.x; }
		inline static const Vector2 CrossVS(const Vector2& v, float s) { return { s * v.y, -s * v.x }; }
		inline static const Vector2 CrossSV(float s, const Vector2& v) { return { -s * v.y, s * v.x }; }
		inline static const double GetRotationDegree(double angle) { return angle * 57.2958; }
		inline static const Vector2 Lerp(const Vector2& a, const Vector2& b, float t) { return { (1.0f - t) * a.x + t * b.x, (1.0f - t) * a.y + t * b.y }; }

		static const Vector2& ZERO;
		static const Vector2& RIGHT;
		static const Vector2& UP;
		static const Vector2& DOWN;
		static const Vector2& LEFT;
	};

	struct AABB
	{
		Vector2 min, max;
	};

	struct Mat22
	{
		Vector2 cx, cy;
	};

	struct Transform {
		inline Transform() : Transform(Vector2(), Rot(), Vector2(10.0f, 10.0f)) {}
		inline Transform(const Vector2& _pos, Rot _rot, Vector2 _size) : position(_pos), rotation(_rot) {}

		Vector2 position;
		Rot rotation;
		Vector2 size;

		// Add move/copy constructors if needed
		Transform(const Transform&) = default;

		inline void UpdateState(const Vector2& _pos, Rot _rot, const Vector2& _size) { position = _pos; rotation = _rot; }

		inline Vector2 GetPosition() const { return position; }
		inline Rot GetRotation() const { return rotation; }
		inline Vector2 GetSize() const { return size; }

		// Setters
		inline void SetPosition(const Vector2& _pos) { position = _pos; }
		inline void SetRotation(Rot _rot) { rotation = _rot; }
		inline void SetRotation(float _rot) { rotation.SetRadians(_rot); }
		inline void SetSize(const Vector2& _size) { size = _size; }
	};
}