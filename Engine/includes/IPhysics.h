//#pragma once
//#include <vector>
//
//struct b2BodyId;
//struct b2ShapeId;
//namespace buki
//{
//	struct WorldId;
//	struct BodyId;
//	struct ShapeId;
//	struct Vector2;
//	struct Entity;
//
//	struct IPhysics
//	{
//		virtual ~IPhysics() = default;
//
//		virtual void LinearImpulse(Entity* _entity, const Vector2 _impulse, const bool _wake) = 0;
//		virtual void SetAwake(const BodyId _id, const bool _state) = 0;
//		virtual bool IsAwake(const BodyId _id) const = 0;
//		virtual void Step(float dt) = 0; // Step the physics world by delta time
//
//		virtual void IntergrateTransform(Entity* _entity) = 0;
//
//		virtual b2BodyId Getb2BodyId(const BodyId _id) const = 0;
//
//		virtual b2ShapeId Getb2ShapeId(const ShapeId _id) const = 0;
//
//		virtual WorldId GetPhysicsWorld() const = 0;
//
//		virtual BodyId CreatePhysicsBody(Entity* _entity) = 0; // Create the physics body in Box2d World
//
//		virtual void DestroyPhysicsBody(BodyId _id) = 0; // Create the physics body in Box2d World
//
//		virtual WorldId CreateWorld() = 0;
//
//		virtual void SetForce(Entity* _entity, Vector2 _force, bool _wake) = 0;
//
//		virtual Vector2 GetVelocity(Entity* _entity) = 0;
//
//		virtual float GetMass(BodyId _id) = 0; // Get the mass of the entity
//
//		virtual void AddShape(int32_t _id, Entity* _entity) = 0;
//
//		virtual void Listen(Entity* _entity) = 0;
//
//		virtual void* GetUserData(BodyId b) = 0;
//
//		virtual void CastRayClosest(Vector2 origin, Vector2 direction, float maxDistance, std::vector<Entity*>& hitEntities) = 0;
//
//		virtual void Destroy() = 0;
//
//		virtual void Reset() = 0;
//	};
//}
