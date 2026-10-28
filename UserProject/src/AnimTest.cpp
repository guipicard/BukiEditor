#pragma once

#include "AnimTest.h"
#include "Engine.h"
#include "Entity.h"
#include "Animator.h"
#include "AnimationAsset.h"
#include "Box.h"
#include "Circle.h"
#include "Polygon.h"
#include "RigidBody.h"
#include "EKey.h"
#include "BukiContainers.h"
#include "Text.h"

buki::AnimTest::AnimTest(Entity* entity) : MonoBehaviour(entity) {}

void buki::AnimTest::Start()
{
	anim = m_Entity->GetComponent<Animator>();

}

void buki::AnimTest::FixedUpdate(const float dt)
{
}

void buki::AnimTest::Destroy()
{
}

void buki::AnimTest::Update(const float dt)
{
	if (anim == nullptr)
	{
		return;
	}

	if (Input().IsKeyPressed(EKey::EKEY_A))
	{
		anim->SetFlipX(true);
		anim->SetFloat("Speed", 1.0f);
	}
	else if (Input().IsKeyPressed(EKey::EKEY_D))
	{
		anim->SetFlipX(false);
		anim->SetFloat("Speed", 1.0f);
	}
	else
	{
		anim->SetFloat("Speed", 0.0f);
	}
	if (Input().IsKeyDown(EKey::EKEY_SPACE))
	{
		Audio().PlaySFX(jumpSFX);
	}
}

void buki::AnimTest::OnCollisionEnter(Entity* other)
{
	Log().LogMessage("COLLISION ENTER with entity: " + other->GetName());
	Audio().PlaySFX(jumpSFX);
}

void buki::AnimTest::OnCollisionExit(Entity* other)
{
	Log().LogMessage("COLLISION EXIT with entity: " + other->GetName());
}

void buki::AnimTest::OnCollisionHit(Entity* other)
{
	Log().LogMessage("COLLISION HIT with entity: " + other->GetName());
}

void buki::AnimTest::OnSensorEnter(Entity* other)
{
	Log().LogMessage("SENSOR ENTER with entity: " + other->GetName());
}

void buki::AnimTest::OnSensorExit(Entity* other)
{
	Log().LogMessage("SENSOR EXIT with entity: " + other->GetName());
}

void buki::AnimTest::OnSet()
{
}
