#pragma once
#include "AngryBirdController.h"
#include "Engine.h"
#include "RigidBody.h"
#include "Circle.h"
#include "Prefabs.h"
#include "Spawner.h"
#include "Camera2D.h"
#include "BukiContainers.h"
#include "ComponentRegistration.h"

REGISTER_COMPONENT(AngryBirdController, "AngryBirdController");

buki::AngryBirdController::AngryBirdController(Entity* _entity)
	: MonoBehaviour(_entity)
{
	Awake();
}

void buki::AngryBirdController::Awake()
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
	//Spawner* spawner = World().Find("Spawner")->GetComponent<Spawner>();
	anchor = World().Find("anchor");
	anchorPos = anchor->T()->GetPosition();
	clickPos = { 0.0f,0.0f };
	forceMultiplier = 6.0f;
	shotLength = 4.0f;
	aimingSound = Audio().LoadSound("./Audio/AngryBird/Sfx - Slingshot Streched.mp3");
	launchingSound = Audio().LoadSound("./Audio/AngryBird/Sfx - Globe Bird Launch 3.mp3");
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
	debugUpdateCounter++;
}

void buki::AngryBirdController::Update(const float dt)
{
	UIHovered = false;
	for (auto* button : buttonsInScene)
	{
		if (button->IsHovered())
		{
			UIHovered = true;
			break;
		}
	}
	if (Input().IsMouseButtonDown(2))
	{
		Entity* groundEntity = World().Find("Ground");
		Vector2 groundPos = groundEntity->T()->GetPosition();
		Vector2 groundSize = groundEntity->T()->GetSize();
		Spawner* spawner = m_Entity->GetComponent<Spawner>();
		const Vector2 size = Vector2(0.75f, 2.25f);
		Vector2 rectpos = { 0.0f, groundPos.y - (groundSize.y / 2.0f) - (size.x / 2.0f) };
		//spawner->Spawn("StoneSlim", rectpos, size, 1.57079633f);
		//spawner->Spawn("StoneSlim", rectpos, size, 1.57079633f);
		rectpos.y = 0.0f;
		rectpos.x -= size.x;
		spawner->Spawn("StoneSlim", rectpos, size, 0.0f);
		spawner->Spawn("StoneSlim", rectpos, size, 0.0f);
		//rectpos.x += size.x*2;
		//spawner->Spawn("StoneSlim", rectpos, size, 0.0f);
	}

	if (Engine::Get().GetTimeScale() == 0.0f) return;
	Vector2 mousePos;
	Input().GetMousePositionWorld(&mousePos.x, &mousePos.y);
	Vector2 birdPos = m_Entity->T()->GetPosition();
	float birdRadius = m_Entity->T()->GetSize().x;
	std::vector<Entity*> entities;
	bool onUI = Physics().QueryPoint(mousePos, entities, 1 << 8);
	if (Input().IsMouseButtonDown(0) && !onUI)
	{
		if (!thrown)
		{
			aiming = true;
			thrown = true;
			clickPos = mousePos;
			Audio().PlaySFX(aimingSound);
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

json buki::AngryBirdController::Serialize()
{
	json doc;
	return doc;
}

void buki::AngryBirdController::Deserialize(json _doc)
{
}

void buki::AngryBirdController::Set()
{
}

void buki::AngryBirdController::Throw(const Vector2 _v)
{
	m_Entity->ActivatePhysics();
	Physics().LinearImpulse(m_Entity, _v * forceMultiplier, true);
	Audio().PlaySFX(launchingSound);
}

void buki::AngryBirdController::Reset()
{
	m_Entity->DeactivatePhysics();
	m_Entity->T()->SetPosition(anchorPos);
	m_Entity->T()->SetRotation(0.0f);
}

void buki::AngryBirdController::EditorController()
{
	/*Camera2D camera = buki::Engine::Get().GetActiveCamera();
	int scroll = Input().GetMouseWheelDelta();
	bool movingScreen = false;
	if (Input().IsMouseButtonPressed(1))
	{
		movingScreen = true;
	}
	if (Input().IsMouseButtonDown(1))
	{
		int x, y;
		Input().GetMousePositionScreen(&x, &y);
		firstMousePos.x = (float)x;
		firstMousePos.y = (float)y;
	}
	if (Input().IsMouseButtonUp(1))
	{
		movingScreen = false;
	}
	if (movingScreen)
	{
		glm::vec2 posVec = buki::Engine::Get().GetActiveCamera().position;
		Vector2 cameraPos = { posVec.x, posVec.y};
		Vector2 mousePos;
		int x, y;
		Input().GetMousePositionScreen(&x, &y);
		mousePos.x = (float)x;
		mousePos.y = (float)y;
		Vector2 mouseDelta = mousePos - firstMousePos;
		buki::Engine::Get().GetActiveCamera().position = { cameraPos.x - mouseDelta.x, cameraPos.y - mouseDelta.y };
		Input().GetMousePositionScreen(&x, &y);
		firstMousePos.x = (float)x;
		firstMousePos.y = (float)y;
	}
	if (Input().IsKeyDown(EKey::EKEY_H))
	{
		float scale;
		Graphics().SetScale(METRES_TO_PIXELS);
		Graphics().GetScale(&scale);
		buki::Engine::Get().GetActiveCamera().position = { 0.0f, 0.0f };
	}
	float timeScale = buki::Engine::Get().GetTimeScale();
	if (Input().IsKeyDown(EKey::EKEY_SPACE))
	{
		buki::Engine::Get().SetTimeScale(timeScale == 1.0f ? 0.0f : 1.0f);
	}*/
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