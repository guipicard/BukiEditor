//#pragma once
//#include "math.h"
//#include <iostream>
//
//namespace buki
//{
//	const float EPSILON = 0.0005f;
//	struct Vector2
//	{
//		union
//		{
//			float m[1][1];
//			float v[2];
//
//			struct
//			{
//				float x;
//				float y;
//			};
//		};
//		// CTOR
//		Vector2();
//		Vector2(const int& _x, const int& _y);
//		Vector2(const float& _x, const float& _y);
//		// OPERATORS
//		bool operator==(const Vector2& other) const;
//		bool operator!=(const Vector2& other) const;
//		Vector2 operator+(const Vector2& other) const;
//		Vector2& operator+=(const Vector2& other);
//		Vector2 operator-(const Vector2& other) const;
//		Vector2 operator-() const;
//		Vector2& operator-=(const Vector2& other);
//		Vector2 operator*(const float& other) const;
//		Vector2& operator*=(const float& other);
//		Vector2 operator/(const float& other) const;
//		Vector2& operator/=(const float& other);
//		// UTILITY FUNCTIONS
//		Vector2& Abs();
//		Vector2 GetAbs();
//		float Magnitude() const;
//		float MagnitudeSquared() const;
//		Vector2 GetNormalized() const;
//		Vector2& Normalize();
//		static const bool BiasGreaterThan(float a, float b)
//		{
//			const float k_biasRelative = 0.95f;
//			const float k_biasAbsolute = 0.01f;
//			return a >= b * k_biasRelative + a * k_biasAbsolute;
//		}
//
//		static const float Distance(const Vector2& a, const Vector2& b);
//		static const float DistanceSquared(const Vector2& a, const Vector2& b);
//		static const bool NearlyEqual(const Vector2& a, const Vector2& b);
//		inline static const float Dot(const Vector2& A, const Vector2& B) { return (A.x * B.x) + (A.y * B.y); }
//		inline static const float Cross(const Vector2& a, const Vector2& b) { return a.x * b.y - a.y * b.x; }
//		inline static const Vector2 Cross(const Vector2& a, float s) { return Vector2(s * a.y, -s * a.x); }
//		inline static const Vector2 Cross(float s, const Vector2& a) { return Vector2(-s * a.y, s * a.x); }
//		inline static const double GetRotationDegree(double angle) { return angle * 57.2958; }
//		static const Vector2 Lerp(const Vector2& start, const Vector2& end, float alpha);
//		static const Vector2 ProjectOnPlane(const Vector2& velocity, const Vector2& normal);
//
//		// CONSTANTS
//		static const Vector2& ZERO;
//		static const Vector2& RIGHT;
//		static const Vector2& UP;
//		static const Vector2& DOWN;
//		static const Vector2& LEFT;
//	};
//}
