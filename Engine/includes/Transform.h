//#pragma once
//#include "BukiContainers.h"
//#include "RectF.h"
//#include <cstdlib>
//#include <algorithm>
//
//namespace buki {
//	struct Transform {
//		// Move TransformState to private section since it's an implementation detail
//		Transform(const Vector2& _pos = Vector2::ZERO, float _rot = 0.0f, const Vector2& _size = Vector2::ZERO)
//			: 
//			position(_pos),
//			rotation(_rot),
//			size(_size)
//		{}
//
//		// Add move/copy constructors if needed
//		Transform(const Transform&) = default;
//		
//
//		// Update state
//		void UpdateState(const Vector2& _pos, float _rot, const Vector2& _size) {
//			position = _pos;
//			rotation = _rot;
//			size = _size;
//		}
//
//		Vector2 GetPosition() const { return position; }
//		float GetRotation() const { return rotation; }
//		Vector2 GetSize() const { return size; }
//		Vector2 GetSizeMetre() const { return size / METRES_TO_PIXELS; }
//		float GetRadius() const { return size.x * 0.5f; }
//		float GetRadiusMetre() const { return GetRadius() / METRES_TO_PIXELS; }
//		float GetRotationDegree() const { return rotation * RAD_TO_DEG; }
//
//		// Setters
//		void SetPosition(const Vector2& pos) 
//		{
//			position = pos;
//		}
//
//		void SetRotation(float rot) 
//		{
//			rotation = rot;
//		}
//
//		void SetSize(const Vector2& _size) 
//		{
//			size = _size;
//		}
//		
//	private:
//		Vector2 position;
//		float rotation;
//		Vector2 size;
//	};
//}