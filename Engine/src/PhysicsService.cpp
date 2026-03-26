#pragma once
#include "PhysicsService.h"
#include "Engine.h"
#include "Entity.h"
#include "box2d.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "BukiContainers.h"
#include "MonoBehaviour.h"
#include "Shapes.h"
#include <algorithm>
#include <variant>

buki::PhysicsService::PhysicsService()
{
	worldId = CreateWorld();

}

void buki::PhysicsService::LinearImpulse(Entity* _entity, const Vector2 _impulse, const bool _wake)
{
	BodyId bId = _entity->GetComponent<RigidBody>()->GetBodyId();
	b2BodyId b2Id = { bId.index1, bId.world0, bId.revision };
	b2Body_ApplyLinearImpulse(b2Id, b2Vec2{ _impulse.x, _impulse.y }, b2Body_GetPosition(b2Id), _wake);
}

void buki::PhysicsService::SetAwake(const BodyId _id, const bool _state)
{
	b2Body_SetAwake({ _id.index1, _id.world0, _id.revision }, _state);
}

bool buki::PhysicsService::IsAwake(const BodyId _id) const
{
	return b2Body_IsAwake({ _id.index1, _id.world0, _id.revision });
}

void buki::PhysicsService::Step(float dt)
{
	b2WorldId b2wId = { worldId.index, worldId.revision };
	if (!b2World_IsValid(b2wId))
	{
		Engine::GetInstance().Log().LogError("invalid world");
		return;
	}
	b2World_Step(b2wId, dt, 8);
	b2BodyEvents bodyEvents = b2World_GetBodyEvents(b2wId);
	for (int i = 0; i < bodyEvents.moveCount; i++)
	{
		b2BodyId bodyId = bodyEvents.moveEvents[i].bodyId;
		Entity* e = (Entity*)b2Body_GetUserData(bodyId);
		if (e != shapeIdToEntityMap[bodyId.index1])
		{
			Engine::GetInstance().Log().LogError("entity does not match shape id in body move event");
			continue;
		}
		if (!e) {
			Engine::GetInstance().Log().LogError("entity is null in body move event");
			continue;
		}
		if (!e->GetComponent<RigidBody>()) {
			Engine::GetInstance().Log().LogError("entity does not have RigidBody component in body move event");
			continue;
		}
		b2Vec2 pos = b2Body_GetPosition(bodyId);
		b2Rot rot = b2Body_GetRotation(bodyId);

		Transform* t = e->GetTransform();
		t->SetPosition({ pos.x, pos.y });
		t->SetRotation({ rot.c, rot.s });
	}

	b2ContactEvents b2Events = b2World_GetContactEvents(b2wId);
	if (b2Events.beginCount > 0) contactEvents->FillCallbacks(b2Events.beginEvents, contactEvents->OnCollisionEnter);
	if (b2Events.beginCount > 0) contactEvents->FillCallbacks(b2Events.beginEvents, contactEvents->OnCollisionExit);
	if (b2Events.beginCount > 0) contactEvents->FillCallbacks(b2Events.beginEvents, contactEvents->OnCollisionHit);

	contactEvents->Step();
}

b2BodyId buki::PhysicsService::Getb2BodyId(const BodyId _id) const
{
	return { _id.index1, _id.world0, _id.revision };
}

b2ShapeId buki::PhysicsService::Getb2ShapeId(const ShapeId _id) const
{
	return { _id.index1, _id.world0, _id.revision };
}

buki::BodyId buki::PhysicsService::CreatePhysicsBody(Entity* _entity)
{
	RigidBody* rb = _entity->GetComponent<RigidBody>();
	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = (b2BodyType)rb->Type;
	bodyDef.fixedRotation = rb->FixedRotation;
	Vector2 pos = _entity->GetTransform()->GetPosition();
	bodyDef.position = { pos.x, pos.y };
	Rot rot = _entity->GetTransform()->GetRotation();
	bodyDef.rotation = { rot.c, rot.s };
	b2WorldId b2wId = b2WorldId{ worldId.index, worldId.revision };

	bodyDef.userData = _entity; // Set user data to the entity pointer

	b2BodyId bodyId = b2CreateBody(b2wId, &bodyDef);
	b2Body_EnableContactEvents(bodyId, true);
	b2Body_EnableHitEvents(bodyId, true);
	shapeIdToEntityMap.emplace(bodyId.index1, _entity);
	return { bodyId.index1, bodyId.world0, bodyId.revision };
}

void buki::PhysicsService::DestroyPhysicsBody(BodyId _id)
{
	b2BodyId b2id = b2BodyId{ _id.index1, _id.world0, _id.revision };
	b2Body_GetWorld(b2id);
	b2ShapeId s2IdDef = { 0,0,0 };
	b2ShapeId s2IdArray[1] = { s2IdDef };
	b2Body_GetShapes(b2id, s2IdArray, 1);
	b2ShapeId s2Id = s2IdArray[0];
	ShapeId sId = { s2Id.index1, s2Id.world0, s2Id.revision };
	shapeIdToEntityMap.erase(sId.index1);
	b2DestroyBody(b2id);
}

buki::WorldId buki::PhysicsService::GetPhysicsWorld() const
{
	return worldId;
}

buki::WorldId buki::PhysicsService::CreateWorld()
{
	b2WorldDef worldDef = b2DefaultWorldDef();

	worldDef.gravity = { 0.0f, 9.8f };
	//worldDef.contactHertz = 60.0;
	//worldDef.userData = this; // Set user data to nullptr, can be used for custom data
	b2WorldId wId = b2CreateWorld(&worldDef);
	worldId.index = wId.index1;
	worldId.revision = wId.revision;
	contactEvents = new ContactEvents();
	return worldId;
}

void buki::PhysicsService::SetForce(Entity* _entity, Vector2 _force, bool _wake)
{
	BodyId bId = _entity->GetComponent<RigidBody>()->GetBodyId();
	b2Body_ApplyForceToCenter({ bId.index1, bId.world0, bId.revision }, { _force.x, _force.y }, _wake);
}

float buki::PhysicsService::GetMass(BodyId _id)
{
	b2BodyId b2id = { _id.index1, _id.world0, _id.revision };
	return b2Body_GetMass(b2id);
}

buki::Vector2 buki::PhysicsService::GetVelocity(Entity* _entity)
{
	BodyId bId = _entity->GetComponent<RigidBody>()->GetBodyId();
	b2Vec2 b2Id = b2Body_GetLinearVelocity({ bId.index1, bId.world0, bId.revision });
	return { b2Id.x, b2Id.y };
}

void buki::PhysicsService::AddShape(int32_t _id, Entity* _entity)
{
	shapeIdToEntityMap[_id] = _entity;
}

void buki::PhysicsService::Listen(Entity* _entity)
{
	BodyId bId = _entity->GetComponent<RigidBody>()->GetBodyId();
	ShapeId sId = _entity->GetComponentOfType<Shapes>()->GetShapeId();
	b2Body_SetUserData({ bId.index1, bId.world0, bId.revision }, _entity);
	b2Body_EnableContactEvents({ bId.index1, bId.world0, bId.revision }, true);
	b2Shape_EnableContactEvents({ sId.index1, sId.world0, sId.revision }, true);

}

void* buki::PhysicsService::GetUserData(BodyId b)
{
	return b2Body_GetUserData({ b.index1, b.world0, b.revision });
}

bool buki::PhysicsService::CastRayClosest(Vector2 _origin, Vector2 _direction, float _maxDistance, std::vector<Entity*>& _hitEntities)
{
	_hitEntities.clear();

	b2WorldId b2wId = { worldId.index, worldId.revision };

	b2Vec2 b2Origin{ _origin.x, _origin.y };

	b2Vec2 b2Translation{ _direction.x * _maxDistance, _direction.y * _maxDistance };

	b2QueryFilter filter = b2DefaultQueryFilter();

	b2RayResult out = b2World_CastRayClosest(b2wId, b2Origin, b2Translation, filter);
	//b2CastOutput out = b2World_CastRayClosest{ b2wId, b2Origin, b2Translation, filter };

	if (!out.hit) return false;

	b2Vec2 hitPoint = b2Origin + out.fraction * b2Translation;

	b2ShapeId b2SId = out.shapeId;
	b2BodyId b2BId = b2Shape_GetBody(b2SId);
	b2ShapeId nullId = b2_nullShapeId;
	if (b2SId.index1 == nullId.index1 && b2SId.revision == nullId.revision && b2SId.world0 == nullId.world0) return false;

	void* userData = b2Body_GetUserData(b2BId);
	if (!userData) return false;

	Entity* entity = static_cast<Entity*>(userData);
	_hitEntities.push_back(entity);

	return !_hitEntities.empty();
}

bool buki::PhysicsService::QueryPointAll(const Vector2& _point, std::vector<Entity*>& _hitEntities)
{
	_hitEntities.clear();

	b2WorldId b2wId = { worldId.index, worldId.revision };

	b2Vec2 p{ _point.x, _point.y };

	// Tiny AABB around mouse point
	const float epsilon = 0.001f;
	b2AABB aabb;
	aabb.lowerBound = { p.x - epsilon, p.y - epsilon };
	aabb.upperBound = { p.x + epsilon, p.y + epsilon };

	struct Context
	{
		b2Vec2 point;
		std::vector<Entity*>* hits;
	} ctx{ p, &_hitEntities };

	b2QueryFilter filter = b2DefaultQueryFilter();

	auto callback = [](b2ShapeId shapeId, void* user) -> bool
		{
			auto* ctx = static_cast<Context*>(user);

			// Precise point-in-shape test
			if (!b2Shape_TestPoint(shapeId, ctx->point))
				return true; // continue

			b2BodyId bodyId = b2Shape_GetBody(shapeId);
			Entity* e = static_cast<Entity*>(b2Body_GetUserData(bodyId));

			if (e)
			{
				ctx->hits->push_back(e);
				return true; // change to true if you want to find all entities under the mouse, currently it stops at the first hit
			}

			return true; // continue to collect all under the mouse
		};

	b2World_OverlapAABB(b2wId, aabb, filter, callback, &ctx);

	return !_hitEntities.empty();
}

bool buki::PhysicsService::QueryPointAll(const Vector2 _point, std::vector<Entity*>& _hitEntities, const int _filter)
{
	_hitEntities.clear();

	b2WorldId b2wId = { worldId.index, worldId.revision };

	b2Vec2 p{ _point.x, _point.y };

	// Tiny AABB around mouse point
	const float epsilon = 0.001f;
	b2AABB aabb;
	aabb.lowerBound = { p.x - epsilon, p.y - epsilon };
	aabb.upperBound = { p.x + epsilon, p.y + epsilon };

	struct Context
	{
		b2Vec2 point;
		std::vector<Entity*>* hits;
	} ctx{ p, &_hitEntities };

	b2QueryFilter filter = b2DefaultQueryFilter();
	filter.categoryBits = _filter;

	auto callback = [](b2ShapeId shapeId, void* user) -> bool
		{
			auto* ctx = static_cast<Context*>(user);

			// Precise point-in-shape test
			if (!b2Shape_TestPoint(shapeId, ctx->point))
				return true; // continue

			b2BodyId bodyId = b2Shape_GetBody(shapeId);
			Entity* e = static_cast<Entity*>(b2Body_GetUserData(bodyId));

			if (e)
			{
				ctx->hits->push_back(e);
				return true; // change to true if you want to find all entities under the mouse, currently it stops at the first hit
			}

			return true; // continue to collect all under the mouse
		};

	b2World_OverlapAABB(b2wId, aabb, filter, callback, &ctx);

	return !_hitEntities.empty();
}

bool buki::PhysicsService::QueryPoint(const Vector2& _point, std::vector<Entity*>& _hitEntities)
{
	QueryPointAll(_point, _hitEntities);
	if (_hitEntities.empty())
	{
		return false;
	}
	std::vector<Entity*> allEntities = Engine::GetInstance().World().GetEntitiesInWorld();
	for (int i = allEntities.size() - 1; i >= 0; i--)
	{
		if (std::count(_hitEntities.begin(), _hitEntities.end(), allEntities[i]) > 0)
		{
			_hitEntities.clear();
			_hitEntities.push_back(allEntities[i]);
			return true;
		}
	}
}

bool buki::PhysicsService::QueryPoint(const Vector2 _point, std::vector<Entity*>& _hitEntities, const int _filter)
{
	QueryPointAll(_point, _hitEntities, _filter);
	if (_hitEntities.empty())
	{
		return false;
	}
	std::vector<Entity*> allEntities = Engine::GetInstance().World().GetEntitiesInWorld();
	for (int i = allEntities.size() - 1; i >= 0; i--)
	{
		if (std::count(_hitEntities.begin(), _hitEntities.end(), allEntities[i]) > 0)
		{
			_hitEntities.clear();
			_hitEntities.push_back(allEntities[i]);
			return true;
		}
	}
}

bool buki::PhysicsService::TestPoint(ShapeId _id, Vector2 _point)
{
	b2ShapeId b2SId{ _id.index1, _id.world0, _id.revision };
	b2Vec2 b2V2{ _point.x, _point.y };
	return b2Shape_TestPoint(b2SId, b2V2);
}

void buki::PhysicsService::SetFilter(ShapeId _id, const int filter)
{
	b2ShapeId b2SId{ _id.index1, _id.world0, _id.revision };
	b2Filter filterData = b2DefaultFilter();
	filterData.maskBits = filter;
	b2Shape_SetFilter(b2SId, filterData);
}

int buki::PhysicsService::GetType(BodyId _id)
{
	return (int)b2Body_GetType(Getb2BodyId(_id));
}

buki::AABB buki::PhysicsService::GetPhysicsSize(ShapeId _id)
{
	b2AABB b2rect = b2Shape_GetAABB(Getb2ShapeId(_id));
	AABB aabb = { {b2rect.lowerBound.x, b2rect.lowerBound.y}, {b2rect.upperBound.x, b2rect.upperBound.y} };
	return aabb;
}

void buki::PhysicsService::Destroy()
{
	contactEvents->Destroy();
	if (contactEvents != nullptr)
	{
		delete contactEvents;
		contactEvents = nullptr;
	}
	b2DestroyWorld(b2WorldId{ worldId.index, worldId.revision });
	worldId = { 0,0 };
	shapeIdToEntityMap.clear();
}

void buki::PhysicsService::Reset()
{
	Destroy();
	CreateWorld();
}



template<typename T>
void buki::ContactEvents::FillCallbacks(T* instance, std::vector<std::function<void()>>& funcList)
{
	for (int i = 0; i < sizeof(*instance) / sizeof(instance[0]); i++)
	{
		b2ShapeId shapeAId = instance[i].shapeIdA;
		b2ShapeId shapeBId = instance[i].shapeIdB;
		Entity* shapeA = nullptr;
		Entity* shapeB = nullptr;
		if (b2Shape_IsValid(shapeAId))
		{
			shapeA = (Entity*)b2Shape_GetUserData(shapeAId);
		}
		if (b2Shape_IsValid(shapeBId))
		{
			shapeB = (Entity*)b2Shape_GetUserData(shapeBId);
		}
		if (shapeA)
		{
			std::vector<MonoBehaviour*> mbAList = shapeA->GetAllComponentsOfType<MonoBehaviour>();
			if (!mbAList.empty())
			{
				for (auto& mb : mbAList)
				{
					HandleEvent(instance[i], mb, shapeB);
				}
			}
		}
		if (shapeB)
		{
			std::vector<MonoBehaviour*> mbBList = shapeB->GetAllComponentsOfType<MonoBehaviour>();
			if (!mbBList.empty())
			{
				for (auto& mb : mbBList)
				{
					HandleEvent(instance[i], mb, shapeA);
				}
			}
		}
	}
}

void buki::ContactEvents::HandleEvent(CollisionEvent event, MonoBehaviour* mb, Entity* other)
{
	std::visit(overloaded{
		[&](b2ContactBeginTouchEvent&) { OnCollisionEnter.push_back([=]() { mb->OnCollisionEnter(other); }); },
		[&](b2ContactEndTouchEvent&) { OnCollisionExit.push_back([=]() { mb->OnCollisionExit(other); }); },
		[&](b2ContactHitEvent&) { OnCollisionHit.push_back([=]() { mb->OnCollisionHit(other); }); },
		}, event);

}

void buki::ContactEvents::Step()
{
	for (std::function<void()> cb : OnCollisionEnter)
		cb();
	for (std::function<void()> cb : OnCollisionExit)
		cb();
	for (std::function<void()> cb : OnCollisionHit)
		cb();

	OnCollisionEnter.clear();
	OnCollisionExit.clear();
	OnCollisionHit.clear();
}
