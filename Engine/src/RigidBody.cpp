#pragma once
#include "RigidBody.h"
#include "Entity.h"
#include "CircleCollider.h"
#include "BoxCollider.h"
#include "ILogger.h"
#include "ICollision.h"
#include "IWorld.h"
#include <cmath>


void buki::RigidBody::FixedUpdate(float dt)
{
}

void buki::RigidBody::Start()
{
}

void buki::RigidBody::Destroy()
{
}

json buki::RigidBody::Serialize()
{
	json doc;
	doc["Type"] = static_cast<int>(Type);
	doc["motionLocks"] = { motionLocks.linearX, motionLocks.linearY, motionLocks.angularZ };
	return doc;
}

void buki::RigidBody::Deserialize(json _doc)
{
	Type = static_cast<BodyType>(_doc["Type"].get<int>());
	motionLocks = { _doc["motionLocks"][0].get<bool>(), _doc["motionLocks"][1].get<bool>(), _doc["motionLocks"][2].get<bool>() };
}

void buki::RigidBody::Set()
{
}

float buki::RigidBody::GetMass() const
{
	return Physics().GetMass(bodyId);
}

buki::Vector2 buki::RigidBody::GetVelocity() const
{
	return Physics().GetVelocity(m_Entity);
}

void buki::RigidBody::SetForce(Vector2 _force)
{
}
