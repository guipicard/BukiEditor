#pragma once
#include "RigidBody.h"

#include "Entity.h"
#include "ILogger.h"
#include "IWorld.h"
#include "Shapes.h"

void buki::RigidBody::FixedUpdate(float dt)
{
}

void buki::RigidBody::Start()
{
}

void buki::RigidBody::Destroy()
{
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
