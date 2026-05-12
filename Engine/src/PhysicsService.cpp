#pragma once
#include "PhysicsService.h"
#include "Engine.h"
#include "Entity.h"
#include "box2d.h"
#include "RigidBody.h"
#include "BukiContainers.h"
#include "MonoBehaviour.h"
#include "Shapes.h"
#include <algorithm>
#include <variant>
#include <windows.h>
#include <functional>

buki::PhysicsService::PhysicsService()
{
	contactEvents = new ContactEvents();
	sensorEvents = new SensorEvents();
}

void buki::PhysicsService::InitPhysics()
{
	worldId = CreateNewWorld();
}

void buki::PhysicsService::LinearImpulse(Entity* _entity, const Vector2 _impulse, const bool _wake)
{
	BodyId bId = _entity->GetComponent<RigidBody>()->GetBodyId();
	b2BodyId b2Id = { bId.index1, bId.world0, bId.generation };
	b2Body_ApplyLinearImpulse(b2Id, b2Vec2{ _impulse.x, _impulse.y }, b2Body_GetPosition(b2Id), _wake);
}

void buki::PhysicsService::SetAwake(const BodyId _id, const bool _state)
{
	b2Body_SetAwake({ _id.index1, _id.world0, _id.generation }, _state);
}

bool buki::PhysicsService::IsAwake(const BodyId _id) const
{
	return b2Body_IsAwake({ _id.index1, _id.world0, _id.generation });
}

void buki::PhysicsService::Step(float dt)
{
	b2WorldId b2wId = { worldId.index, worldId.generation };
	if (!b2World_IsValid(b2wId))
	{
		Engine::Get().Log().LogError("invalid world");
		return;
	}
	b2World_Step(b2wId, dt, 8);
	b2BodyEvents bodyEvents = b2World_GetBodyEvents(b2wId);
	for (int i = 0; i < bodyEvents.moveCount; ++i)
	{
		const b2BodyMoveEvent ev = bodyEvents.moveEvents[i];
		Entity* e = static_cast<Entity*>(ev.userData);

		if (!e)
		{
			Engine::Get().Log().LogError("entity is null in body move event");
			continue;
		}

		if (!e->GetComponent<RigidBody>())
		{
			Engine::Get().Log().LogError("entity does not have RigidBody component in body move event");
			continue;
		}

		const b2Vec2 pos = ev.transform.p;
		const b2Rot rot = ev.transform.q;

		Transform* t = e->T();
		t->SetPosition({ pos.x, pos.y });
		t->SetRotation({ rot.c, rot.s });
	}

	b2ContactEvents b2ContactEvents = b2World_GetContactEvents(b2wId);
	if (b2ContactEvents.beginCount > 0) contactEvents->FillCollisionCallbacks(b2ContactEvents.beginEvents, b2ContactEvents.beginCount, contactEvents->OnCollisionEnter);
	if (b2ContactEvents.endCount > 0) contactEvents->FillCollisionCallbacks(b2ContactEvents.endEvents, b2ContactEvents.endCount, contactEvents->OnCollisionExit);
	if (b2ContactEvents.hitCount > 0) contactEvents->FillCollisionCallbacks(b2ContactEvents.hitEvents, b2ContactEvents.hitCount, contactEvents->OnCollisionHit);
	contactEvents->CollisionStep();

	b2SensorEvents b2SensorEvents = b2World_GetSensorEvents(b2wId);
	if (b2SensorEvents.beginCount > 0)
	{
		sensorEvents->FillSensorCallbacks(b2SensorEvents.beginEvents, b2SensorEvents.beginCount, sensorEvents->OnSensorEnter);
	}
	if (b2SensorEvents.endCount > 0)
	{
		sensorEvents->FillSensorCallbacks(b2SensorEvents.endEvents, b2SensorEvents.endCount, sensorEvents->OnSensorExit);
	}
	sensorEvents->SensorStep();
}

b2BodyId buki::PhysicsService::Getb2BodyId(const BodyId _id) const
{
	return { _id.index1, _id.world0, _id.generation };
}

b2ShapeId buki::PhysicsService::Getb2ShapeId(const ShapeId _id) const
{
	return { _id.index1, _id.world0, _id.generation };
}

buki::BodyId buki::PhysicsService::CreatePhysicsBody(Entity* entity)
{
	if (entity == nullptr)
	{
		return {};
	}

	RigidBody* rb = entity->GetComponent<RigidBody>();
	if (rb == nullptr)
	{
		return {};
	}

	const RigidBodyDef& def = rb->def;

	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = static_cast<b2BodyType>(def.type);
	bodyDef.motionLocks =
	{
		def.motionLocks.linearX,
		def.motionLocks.linearY,
		def.motionLocks.angularZ
	};

	const Vector2 pos = entity->T()->GetPosition();
	bodyDef.position = { pos.x, pos.y };

	const Rot rot = entity->T()->GetRotation();
	bodyDef.rotation = { rot.c, rot.s };


	const b2WorldId b2World = { worldId.index, worldId.generation };
	const b2BodyId b2Body = b2CreateBody(b2World, &bodyDef);



	return { b2Body.index1, b2Body.world0, b2Body.generation };
}

void buki::PhysicsService::DestroyPhysicsBody(BodyId id)
{
	const b2BodyId b2Id = { id.index1, id.world0, id.generation };
	if (!b2Body_IsValid(b2Id))
	{
		return;
	}

	b2DestroyBody(b2Id);
}

buki::WorldId buki::PhysicsService::GetPhysicsWorld() const
{
	return worldId;
}

buki::WorldId buki::PhysicsService::CreateNewWorld()
{
	if (worldId.index != 0 && worldId.generation != 0)
	{
		Destroy();
	}
	b2WorldDef worldDef = b2DefaultWorldDef();

	worldDef.gravity = { 0.0f, 9.8f };
	//worldDef.contactHertz = 60.0;
	//worldDef.userData = this; // Set user data to nullptr, can be used for custom data
	b2WorldId wId = b2CreateWorld(&worldDef);
	worldId = { wId.index1, wId.generation };
	if (contactEvents == nullptr) contactEvents = new ContactEvents();
	if (sensorEvents == nullptr) sensorEvents = new SensorEvents();
	return worldId;
}

void buki::PhysicsService::SetForce(Entity* _entity, Vector2 _force, bool _wake)
{
	BodyId bId = _entity->GetComponent<RigidBody>()->GetBodyId();
	b2Body_ApplyForceToCenter({ bId.index1, bId.world0, bId.generation }, { _force.x, _force.y }, _wake);
}

float buki::PhysicsService::GetMass(BodyId _id)
{
	b2BodyId b2id = { _id.index1, _id.world0, _id.generation };
	return b2Body_GetMass(b2id);
}

buki::Vector2 buki::PhysicsService::GetVelocity(Entity* _entity)
{
	BodyId bId = _entity->GetComponent<RigidBody>()->GetBodyId();
	b2Vec2 b2Id = b2Body_GetLinearVelocity({ bId.index1, bId.world0, bId.generation });
	return { b2Id.x, b2Id.y };
}

void buki::PhysicsService::Listen(Entity* _entity)
{
	BodyId bId = _entity->GetComponent<RigidBody>()->GetBodyId();
	ShapeId sId = _entity->GetComponentOfType<Shapes>()->GetShapeId();
	b2BodyId b2Body = { bId.index1, bId.world0, bId.generation };
	b2ShapeId b2Shape = { sId.index1, sId.world0, sId.generation };

	b2Body_SetUserData(b2Body, _entity);
	b2Body_SetName(b2Body, _entity->GetName().c_str());
	b2Body_EnableContactEvents(b2Body, true);
	b2Body_EnableHitEvents(b2Body, true);

	b2Shape_EnableContactEvents(b2Shape, true);
	b2Shape_EnableSensorEvents(b2Shape, true);
	b2Shape_EnableHitEvents(b2Shape, true);
	b2Shape_EnablePreSolveEvents(b2Shape, true);
}

void* buki::PhysicsService::GetUserData(BodyId b)
{
	return b2Body_GetUserData({ b.index1, b.world0, b.generation });
}

bool buki::PhysicsService::CastRayClosest(Vector2 _origin, Vector2 _direction, float _maxDistance, std::vector<Entity*>& _hitEntities)
{
	_hitEntities.clear();

	b2WorldId b2wId = { worldId.index, worldId.generation };

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
	if (b2SId.index1 == nullId.index1 && b2SId.generation == nullId.generation && b2SId.world0 == nullId.world0) return false;

	void* userData = b2Body_GetUserData(b2BId);
	if (!userData) return false;

	Entity* entity = static_cast<Entity*>(userData);
	_hitEntities.push_back(entity);

	return !_hitEntities.empty();
}

bool buki::PhysicsService::QueryPointAll(const Vector2& _point, std::vector<Entity*>& _hitEntities)
{
	_hitEntities.clear();

	b2WorldId b2wId = { worldId.index, worldId.generation };

	b2Vec2 p{ _point.x, _point.y };

	// Tiny AABB around mouse point
	const float epsilon = 0.001f;
	b2AABB aabb{
	{ p.x - epsilon, p.y - epsilon },
	{ p.x + epsilon, p.y + epsilon }
	};

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

	b2WorldId b2wId = { worldId.index, worldId.generation };

	b2Vec2 p{ _point.x, _point.y };

	// Tiny AABB around mouse point
	const float epsilon = 0.001f;
	b2AABB aabb{
	{ p.x - epsilon, p.y - epsilon },
	{ p.x + epsilon, p.y + epsilon }
	};

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
	std::vector<Entity*> allEntities = Engine::Get().World().GetEntitiesInWorld();
	for (int i = (int)allEntities.size() - 1; i >= 0; i--)
	{
		if (std::count(_hitEntities.begin(), _hitEntities.end(), allEntities[i]) > 0)
		{
			_hitEntities.clear();
			_hitEntities.push_back(allEntities[i]);
			return true;
		}
	}
	return false;
}

bool buki::PhysicsService::QueryPoint(const Vector2 _point, std::vector<Entity*>& _hitEntities, const int _filter)
{
	QueryPointAll(_point, _hitEntities, _filter);
	if (_hitEntities.empty())
	{
		return false;
	}
	std::vector<Entity*> allEntities = Engine::Get().World().GetEntitiesInWorld();
	for (int i = (int)allEntities.size() - 1; i >= 0; i--)
	{
		if (std::count(_hitEntities.begin(), _hitEntities.end(), allEntities[i]) > 0)
		{
			_hitEntities.clear();
			_hitEntities.push_back(allEntities[i]);
			return true;
		}
	}
	return false;
}

bool buki::PhysicsService::TestPoint(ShapeId _id, Vector2 _point)
{
	b2ShapeId b2SId{ _id.index1, _id.world0, _id.generation };
	b2Vec2 b2V2{ _point.x, _point.y };
	return b2Shape_TestPoint(b2SId, b2V2);
}

void buki::PhysicsService::SetFilter(ShapeId _id, const int filter)
{
	b2ShapeId b2SId{ _id.index1, _id.world0, _id.generation };
	b2Filter filterData = b2DefaultFilter();
	filterData.maskBits = filter;
	b2Shape_SetFilter(b2SId, filterData);
}

int buki::PhysicsService::GetType(BodyId _id)const
{
	return (int)b2Body_GetType(Getb2BodyId(_id));
}

buki::AABB buki::PhysicsService::GetPhysicsSize(ShapeId _id) const
{
	b2AABB b2rect = b2Shape_GetAABB(Getb2ShapeId(_id));
	AABB aabb = { {b2rect.lowerBound.x, b2rect.lowerBound.y}, {b2rect.upperBound.x, b2rect.upperBound.y} };
	return aabb;
}

void buki::PhysicsService::Destroy()
{
	if (contactEvents != nullptr)
	{
		contactEvents->Destroy();
		delete contactEvents;
		contactEvents = nullptr;
	}
	if (sensorEvents != nullptr)
	{
		sensorEvents->Destroy();
		delete sensorEvents;
		sensorEvents = nullptr;
	}
	if (b2World_IsValid({ worldId.index, worldId.generation }))
	{
		b2DestroyWorld(b2WorldId{ worldId.index, worldId.generation });
	}
	worldId = { 0,0 };
}

template<typename T>
void buki::ContactEvents::FillCollisionCallbacks(T* instance, int count, std::vector<std::function<void()>>& funcList)
{
	for (int i = 0; i < count; i++)
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
					HandleCollisionEvent(instance[i], mb, shapeB);
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
					HandleCollisionEvent(instance[i], mb, shapeA);
				}
			}
		}
	}
}

void buki::ContactEvents::HandleCollisionEvent(CollisionEvent event, MonoBehaviour* mb, Entity* other)
{
	std::visit(overloaded{
		[&](b2ContactBeginTouchEvent&) { OnCollisionEnter.push_back([=]() { mb->OnCollisionEnter(other); }); },
		[&](b2ContactEndTouchEvent&) { OnCollisionExit.push_back([=]() { mb->OnCollisionExit(other); }); },
		[&](b2ContactHitEvent&) { OnCollisionHit.push_back([=]() { mb->OnCollisionHit(other); }); },
		}, event);
}

void buki::ContactEvents::CollisionStep()
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

template<typename T>
void buki::SensorEvents::FillSensorCallbacks(T* instance, int count, std::vector<std::function<void()>>& funcList)
{
	for (int i = 0; i < count; i++)
	{
		b2ShapeId sensorShapeId = instance[i].sensorShapeId;
		b2ShapeId visitorShapeId = instance[i].visitorShapeId;
		Entity* sensorShape = nullptr;
		Entity* visitorShape = nullptr;
		if (b2Shape_IsValid(sensorShapeId))
		{
			sensorShape = (Entity*)b2Shape_GetUserData(sensorShapeId);
		}
		if (b2Shape_IsValid(visitorShapeId))
		{
			visitorShape = (Entity*)b2Shape_GetUserData(visitorShapeId);
		}
		if (sensorShape)
		{
			std::vector<MonoBehaviour*> mbAList = sensorShape->GetAllComponentsOfType<MonoBehaviour>();
			if (!mbAList.empty())
			{
				for (auto& mb : mbAList)
				{
					HandleSensorEvent(instance[i], mb, visitorShape);
				}
			}
		}
		if (visitorShape)
		{
			std::vector<MonoBehaviour*> mbBList = visitorShape->GetAllComponentsOfType<MonoBehaviour>();
			if (!mbBList.empty())
			{
				for (auto& mb : mbBList)
				{
					HandleSensorEvent(instance[i], mb, sensorShape);
				}
			}
		}
	}
}

void buki::SensorEvents::HandleSensorEvent(TriggerEvent event, MonoBehaviour* mb, Entity* other)
{
	std::visit(overloaded{
		[&](b2SensorBeginTouchEvent&) { OnSensorEnter.push_back([=]() { mb->OnSensorEnter(other); }); },
		[&](b2SensorEndTouchEvent&) { OnSensorExit.push_back([=]() { mb->OnSensorExit(other); }); },
		}, event);
}

void buki::SensorEvents::SensorStep()
{
	for (std::function<void()> cb : OnSensorEnter)
		cb();
	for (std::function<void()> cb : OnSensorExit)
		cb();

	for (auto func : OnSensorEnter)
	{
		delete& func;
	}
	for (auto func : OnSensorExit)
	{
		delete& func;
	}
	OnSensorEnter.clear();
	OnSensorExit.clear();
}