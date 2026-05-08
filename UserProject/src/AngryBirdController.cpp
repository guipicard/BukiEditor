#pragma once
#include "AngryBirdController.h"
#include "Entity.h"
#include "Engine.h"
#include "RigidBody.h"
#include "Circle.h"
#include "Prefabs.h"
#include "Spawner.h"
#include "Camera2D.h"
#include "BukiContainers.h"
#include "Button.h"
//#include "ComponentRegistration.h"
//
//REGISTER_COMPONENT(AngryBirdController, "AngryBirdController");

buki::AngryBirdController::AngryBirdController(Entity* _entity)
	: MonoBehaviour(_entity)
{
}

void buki::AngryBirdController::Start()
{
	Spawner* spawner = m_Entity->GetComponent<Spawner>();
	if (spawner == nullptr)
	{
		spawner = m_Entity->AddComponent<Spawner>();
	}
	spawner->AddPrototype("StoneSlim", new StoneSlim());
	anchor = World().FindEntityByName("anchor");
	anchorPos = anchor->T()->GetPosition();
	aimingSoundHandle = Audio().LoadSound("./Audio/AngryBird/Sfx - Slingshot Streched.mp3");
	launchingSoundHandle = Audio().LoadSound("./Audio/AngryBird/Sfx - Globe Bird Launch 3.mp3");
	AddCollisionSound("./Audio/AngryBird/Sfx - Globe Bird Hit 1.mp3");
	AddCollisionSound("./Audio/AngryBird/Sfx - Globe Bird Hit 2.mp3");
	AddCollisionSound("./Audio/AngryBird/Sfx - Globe Bird Hit 3.mp3");

	for (auto* e : World().GetEntitiesInWorld())
	{
		if (e->GetComponent<Button>())
		{
			buttonsInScene.push_back(e->GetComponent<Button>());
		}
	}
}

void buki::AngryBirdController::Destroy()
{
}

void buki::AngryBirdController::FixedUpdate(const float dt)
{
}

void buki::AngryBirdController::Update(const float dt)
{
	bool UIHovered = false;
	for (Button* button : buttonsInScene)
	{
		if (button->IsHovered())
		{
			UIHovered = true;
			break;
		}
	}

	if (Engine::Get().GetTimeScale() == 0.0f) return;
	Vector2 mousePos;
	Input().GetMousePositionWorld(&mousePos.x, &mousePos.y);
	Vector2 birdPos = m_Entity->T()->GetPosition();
	float birdRadius = m_Entity->T()->GetSize().x;
	std::vector<Entity*> entities;
	if (Input().IsMouseButtonDown(0) && !UIHovered)
	{
		if (!thrown)
		{
			aiming = true;
			thrown = true;
			clickPos = mousePos;
			Audio().PlaySFX(aimingSoundHandle);
		}
		else
		{
			Reset();
			thrown = false;
		}
	}
	if (aiming)
	{
		if (Input().IsMouseButtonUp(0))
		{
			if (aiming)
			{
				aiming = false;
				clickPos = { 0.0f,0.0f };
				Throw(anchorPos - m_Entity->T()->GetPosition());
			}
		}
		else
		{
			Vector2 offset = clickPos - mousePos;
			if (offset.Length() > shotLength) offset = offset.GetNormalized() * shotLength;
			Vector2 slignPos = anchorPos - offset;
			m_Entity->T()->SetPosition(slignPos);
		}
	}
}

void buki::AngryBirdController::OnCollisionEnter(Entity* other)
{
	if (other == nullptr) return;
	RigidBody* rbA = other->GetComponent<RigidBody>();
	RigidBody* rbB = m_Entity->GetComponent<RigidBody>();
	if (rbA)
	{
		Vector2 velA = rbA->GetVelocity();
		Vector2 velB = rbB->GetVelocity();
		float massA = rbA->GetMass();
		float massB = rbB->GetMass();
		Vector2 vel = velA - velB;
		float mass = massA + massB;
		float dmg = (vel.Length() * mass);
		if (dmg > soundDamageThreshold)
		{
			PlayCollisionSound();
		}
	}
}

void buki::AngryBirdController::OnCollisionExit(Entity* other)
{
}

void buki::AngryBirdController::OnCollisionHit(Entity* other)
{
	Log().LogMessage("Collision HIT with: " + other->GetName());
}

void buki::AngryBirdController::OnSensorEnter(Entity* other)
{
}

void buki::AngryBirdController::OnSensorExit(Entity* other)
{
}

void buki::AngryBirdController::OnSet()
{
}

void buki::AngryBirdController::Throw(const Vector2 _v)
{
	m_Entity->ActivatePhysics();
	Physics().LinearImpulse(m_Entity, _v * forceMultiplier, true);
	Audio().PlaySFX(launchingSoundHandle);
}

void buki::AngryBirdController::Reset()
{
	m_Entity->DeactivatePhysics();
	m_Entity->T()->SetPosition(anchorPos);
	m_Entity->T()->SetRotation(0.0f);
}

void buki::AngryBirdController::AddCollisionSound(const std::string& sound)
{
	size_t id = buki::Engine::Get().Audio().LoadSound(sound);
	collisionSounds.push_back(id);
}

void buki::AngryBirdController::PlayCollisionSound()
{
	if (collisionSounds.empty()) return;

	int index = rand() % collisionSounds.size();
	Audio().PlaySFX(collisionSounds[index]);
}