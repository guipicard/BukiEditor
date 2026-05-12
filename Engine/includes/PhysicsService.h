#pragma once
#include "stdint.h"
#include <vector>
#include <map>
#include <functional>
#include <variant>


struct b2BodyId;
struct b2ShapeId;

struct b2ContactBeginTouchEvent;
struct b2ContactEndTouchEvent;
struct b2ContactHitEvent;
struct b2SensorBeginTouchEvent;
struct b2SensorEndTouchEvent;

namespace buki
{
	class Entity;
	struct ShapeId;
	struct BodyId;
	class Vector2;
	class MonoBehaviour;
	struct AABB;

	struct WorldId
	{
		uint16_t index;
		uint16_t generation;
	};

	struct ContactData
	{
		Entity* shapeA;
		Entity* shapeB;
	};

	using CollisionEvent = std::variant<b2ContactBeginTouchEvent, b2ContactEndTouchEvent, b2ContactHitEvent>;
	using TriggerEvent = std::variant<b2SensorBeginTouchEvent, b2SensorEndTouchEvent>;
	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

	struct ContactEvents
	{
		ContactEvents() = default;
		template<typename T>
		void FillCollisionCallbacks(T* instance, int count, std::vector<std::function<void()>>& funcList);

		void Destroy() {
			if (!OnCollisionEnter.empty()) OnCollisionEnter.clear();
			if (!OnCollisionExit.empty()) OnCollisionExit.clear();
			if (!OnCollisionHit.empty()) OnCollisionHit.clear();
			delete& OnCollisionEnter;
			delete& OnCollisionExit;
			delete& OnCollisionHit;
		}
		void HandleCollisionEvent(CollisionEvent event, MonoBehaviour* mb, Entity* other);
		void CollisionStep();

		std::vector<std::function<void()>>& OnCollisionEnter = *new std::vector<std::function<void()>>();
		std::vector<std::function<void()>>& OnCollisionExit = *new std::vector<std::function<void()>>();
		std::vector<std::function<void()>>& OnCollisionHit = *new std::vector<std::function<void()>>();
	};

	struct SensorEvents
	{
		SensorEvents() = default;
		template<typename T>
		void FillSensorCallbacks(T* instance, int count, std::vector<std::function<void()>>& funcList);

		void Destroy() {
			if (OnSensorEnter.size() > 0)OnSensorEnter.clear();
			if (OnSensorExit.size() > 0)OnSensorExit.clear();
			delete& OnSensorEnter;
			delete& OnSensorExit;
		}
		void HandleSensorEvent(TriggerEvent event, MonoBehaviour* mb, Entity* other);
		void SensorStep();

		std::vector<std::function<void()>>& OnSensorEnter = *new std::vector<std::function<void()>>();
		std::vector<std::function<void()>>& OnSensorExit = *new std::vector<std::function<void()>>();
	};

	class PhysicsService
	{
	public:
		PhysicsService();
		void InitPhysics();
		void LinearImpulse(Entity* _entity, const Vector2 _impulse, const bool _wake);
		void SetAwake(const BodyId _id, const bool _state);
		bool IsAwake(const BodyId _id) const;
		void Step(const float dt);
		b2BodyId Getb2BodyId(const BodyId _id) const;
		b2ShapeId Getb2ShapeId(const ShapeId _id) const;
		WorldId GetPhysicsWorld() const;
		BodyId CreatePhysicsBody(Entity* _entity);
		void DestroyPhysicsBody(BodyId _id);
		WorldId CreateNewWorld();
		void SetForce(Entity* _entity, Vector2 _force, bool _wake);
		float GetMass(BodyId _id);
		Vector2 GetVelocity(Entity* _entity);
		void Listen(Entity* _entity);
		void* GetUserData(BodyId b);
		bool CastRayClosest(Vector2 origin, Vector2 direction, float maxDistance, std::vector<Entity*>& hitEntities);
		bool QueryPointAll(const Vector2& _point, std::vector<Entity*>& _hitEntities);
		bool QueryPointAll(const Vector2 _point, std::vector<Entity*>& _hitEntities, const int _filter);
		bool QueryPoint(const Vector2& _point, std::vector<Entity*>& _hitEntities);
		bool QueryPoint(const Vector2 _point, std::vector<Entity*>& _hitEntities, const int _filter);
		bool TestPoint(ShapeId _id, Vector2 _point);
		void SetFilter(ShapeId _id, const int filter);
		int GetType(BodyId _id) const;
		AABB GetPhysicsSize(ShapeId _id) const;
		void Destroy();
	private:
		WorldId worldId = {0,0};
		ContactEvents* contactEvents;
		SensorEvents* sensorEvents;
	};
}