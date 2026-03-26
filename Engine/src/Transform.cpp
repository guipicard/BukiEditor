//#pragma once
//#include "Transform.h"
//
//buki::Transform::Transform(Vector2 _pos, float _rot, Vector2 _size)
//	: position(_pos),
//	rotationRadient(_rot),
//	size(_size)
//{
//	u = Matrix2(_rot);
//}
//
//buki::Transform::~Transform()
//{
//}
//
//buki::AABB buki::Transform::GetAABB() const
//{
//	Vector2 halfSize = size / 2.0f;
//
//	{ return AABB(position - halfSize, position + halfSize); }
//}
//
//void buki::Transform::GetBoxVertices(Vector2 _list[4]) const
//{
//	Vector2 boxHalfSize = GetSize()/2.0f;
//	
//	_list[0] = -boxHalfSize;
//	_list[1] = { boxHalfSize.x, -boxHalfSize.y };
//	_list[2] = boxHalfSize;
//	_list[3] = { -boxHalfSize.x, boxHalfSize.y };
//}
//
//buki::Vector2 buki::Transform::GetSupport(const Vector2& dir)
//{
//	float bestProjection = -FLT_MAX;
//	Vector2 bestVertex;
//	Vector2 vertices[4];
//	GetBoxVertices(vertices);
//	for (int i = 0; i < 4; ++i)
//	{
//		Vector2 v = vertices[i];
//		float projection = Vector2::Dot(v, dir);
//
//		if (projection > bestProjection)
//		{
//			bestVertex = v;
//			bestProjection = projection;
//		}
//	}
//
//	return bestVertex;
//}
