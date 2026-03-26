//#include "Vector2.h"
//
//using namespace buki;
//
//const Vector2& Vector2::ZERO = Vector2(0.0f, 0.0f);
//const Vector2& Vector2::RIGHT = Vector2(1.0f, 0.0f);
//const Vector2& Vector2::UP = Vector2(0.0f, -1.0f);
//const Vector2& Vector2::LEFT = Vector2(-1.0f, 0.0f);
//const Vector2& Vector2::DOWN = Vector2(0.0f,1.0f);
//
//Vector2::Vector2() 
//	: Vector2(0.0f, 0.0f) {}
//
//Vector2::Vector2(const int& _x, const int& _y) 
//	: x(static_cast<float>(_x)), y(static_cast<float>(_y)) {}
//
//Vector2::Vector2(const float& _x, const float& _y) 
//	: x(_x), y(_y) {}
//
//bool Vector2::operator==(const Vector2& other) const
//{
//	return x == other.x && y == other.y;
//}
//
//bool Vector2::operator!=(const Vector2& other) const
//{
//	return x != other.x || y != other.y;
//}
//
//Vector2 Vector2::operator+(const Vector2& other) const
//{
//	return Vector2(x + other.x, y + other.y);
//}
//
//Vector2& Vector2::operator+=(const Vector2& other)
//{
//	x += other.x, y += other.y;
//	return *this;
//}
//
//Vector2 Vector2::operator-(const Vector2& other) const
//{
//	return Vector2(x - other.x, y - other.y);
//}
//
//Vector2 buki::Vector2::operator-() const
//{
//	return Vector2(-x, -y);
//}
//
//Vector2& Vector2::operator-=(const Vector2& other)
//{
//	x = x - other.x, y = y - other.y;
//	return *this;
//}
//
//Vector2 Vector2::operator*(const float& other) const
//{
//	return Vector2(x * other, y * other);
//}
//
//Vector2& Vector2::operator*=(const float& other)
//{
//	x *= other; y *= other;
//	return *this;
//}
//
//Vector2 Vector2::operator/(const float& other) const
//{
//	return Vector2(x / other, y / other);
//}
//
//Vector2& Vector2::operator/=(const float& other)
//{
//	x /= other; y /= other;
//	return *this;
//}
//
//Vector2& Vector2::Abs()
//{
//	x = abs(x);
//	y = abs(y);
//	return *this;
//}
//
//Vector2 buki::Vector2::GetAbs()
//{
//	return Vector2(abs(x), abs(y));
//}
//
//float Vector2::Magnitude() const
//{
//	return sqrt(x * x + y * y);
//}
//
//float Vector2::MagnitudeSquared() const
//{
//	return x * x + y * y;
//}
//
//Vector2 Vector2::GetNormalized() const
//{
//	if (x == 0.0f) return Vector2(0.0f, y/abs(y));
//	if (y == 0.0f) return Vector2(x/abs(x), 0.0f);
//	const float mag = Magnitude();
//	return *this / mag;
//}
//
//Vector2& Vector2::Normalize()
//{
//	if (x + y == 0.0f)
//	{
//		return *this;
//	}
//	if (x == 0.0f) {
//		y /= abs(y);
//		return *this;
//	}
//	if (y == 0.0f) {
//		x /= abs(x);
//		return *this;
//	}
//	const float mag = Magnitude();
//	return *this /= mag;
//}
//
//const float buki::Vector2::Distance(const Vector2& a, const Vector2& b)
//{
//	return static_cast<float>(sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)));
//	
//}
//
//const float buki::Vector2::DistanceSquared(const Vector2& a, const Vector2& b)
//{
//	return static_cast<float>(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
//}
//
//const bool buki::Vector2::NearlyEqual(const Vector2& a, const Vector2& b)
//{
//	return DistanceSquared(a,b) < EPSILON;
//}
//
//const buki::Vector2 buki::Vector2::Lerp(const Vector2& start, const Vector2& end, float alpha)
//{
//	return Vector2(
//		start.x + (end.x - start.x) * alpha,
//		start.y + (end.y - start.y) * alpha
//	);
//}
//
//const Vector2 buki::Vector2::ProjectOnPlane(const Vector2& velocity, const Vector2& normal)
//{
//	return velocity - (normal * Vector2::Dot(velocity, normal));
//}
