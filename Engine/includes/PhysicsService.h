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


namespace buki
{
	using CollisionEvent = std::variant<b2ContactBeginTouchEvent, b2ContactEndTouchEvent, b2ContactHitEvent>;
	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

	struct Entity;
	struct WorldId;
	struct ShapeId;
	struct BodyId;
	struct Vector2;
	struct Entity;
	struct MonoBehaviour;
	struct AABB;

	struct WorldId
	{
		uint16_t index;
		uint16_t revision;
	};

	struct ContactData
	{
		Entity* shapeA;
		Entity* shapeB;
	};

	struct ContactEvents
	{
		ContactEvents() = default;
		template<typename T>
		void FillCallbacks(T* instance, std::vector<std::function<void()>>& funcList);

		void Destroy() {
			OnCollisionEnter.clear();
			OnCollisionExit.clear();
			OnCollisionHit.clear();
			delete& OnCollisionEnter;
			delete& OnCollisionExit;
			delete& OnCollisionHit;
		}
		void HandleEvent(CollisionEvent event, MonoBehaviour* mb, Entity* other);
		void Step();

		/*std::vector<std::function<void(Entity* other)>> GetCollisionEnterCallbacks() {
			return OnCollisionEnter;
		}

		std::vector<std::function<void(Entity* other)>> GetCollisionExitCallbacks() {
			return OnCollisionExit;
		}

		std::vector<std::function<void(Entity* other)>> GetCollisionHitCallbacks() {
			return OnCollisionHit;
		}*/

		//private:
		std::vector<std::function<void()>>& OnCollisionEnter = *new std::vector<std::function<void()>>();
		std::vector<std::function<void()>>& OnCollisionExit = *new std::vector<std::function<void()>>();
		std::vector<std::function<void()>>& OnCollisionHit = *new std::vector<std::function<void()>>();

	};

	struct PhysicsService
	{
		PhysicsService();
		void LinearImpulse(Entity* _entity, const Vector2 _impulse, const bool _wake);
		void SetAwake(const BodyId _id, const bool _state);
		bool IsAwake(const BodyId _id) const;
		void Step(float dt);
		b2BodyId Getb2BodyId(const BodyId _id) const;
		b2ShapeId Getb2ShapeId(const ShapeId _id) const;
		WorldId GetPhysicsWorld() const;
		BodyId CreatePhysicsBody(Entity* _entity);
		void DestroyPhysicsBody(BodyId _id);
		WorldId CreateWorld();
		void SetForce(Entity* _entity, Vector2 _force, bool _wake);
		float GetMass(BodyId _id);
		Vector2 GetVelocity(Entity* _entity);
		void AddShape(int32_t _id, Entity* _entity);
		void Listen(Entity* _entity);
		void* GetUserData(BodyId b);
		bool CastRayClosest(Vector2 origin, Vector2 direction, float maxDistance, std::vector<Entity*>& hitEntities);
		bool QueryPointAll(const Vector2& _point, std::vector<Entity*>& _hitEntities);
		bool QueryPointAll(const Vector2 _point, std::vector<Entity*>& _hitEntities, const int _filter);
		bool QueryPoint(const Vector2& _point, std::vector<Entity*>& _hitEntities);
		bool QueryPoint(const Vector2 _point, std::vector<Entity*>& _hitEntities, const int _filter);
		bool TestPoint(ShapeId _id, Vector2 _point);
		void SetFilter(ShapeId _id, const int filter);
		int GetType(BodyId _id);
		AABB GetPhysicsSize(ShapeId _id);
		void Destroy();
		void Reset();
	private:
		WorldId worldId;
		std::map<int32_t, Entity*> shapeIdToEntityMap;
		ContactEvents* contactEvents;
	};

}