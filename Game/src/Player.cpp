#pragma once
#include "Player.h"
#include "Animation.h"
#include "Engine.h"
#include "Ekey.h"
#include "RigidBody.h"
#include "Box.h"
#include "Entity.h"
#include "Camera.h"
#include "PlayerState.h"
#include "PlayerIdle.h"
#include "PlayerWalk.h"
#include "PlayerRun.h"
#include "PlayerJump.h"
#include "Text.h"
#include "EditorController.h"
#include "ComponentRegistration.h"

REGISTER_COMPONENT(Player, "Player");

buki::Player::Player(Entity* entity) : MonoBehaviour(entity)
{
	collider = nullptr;
	playerAnim = nullptr;
	rigidbody = nullptr;
	currentState = nullptr;
	debugText = nullptr;
	playerSize = { 1.0, 0.9f };
}

void buki::Player::SetState(const std::string& state)
{
	if (states.count(state) > 0)
	{
		if (currentState != nullptr)
		{
			currentState->Exit(this);
		}
		currentState = states[state];
		currentState->Start(this);
	}
}

void buki::Player::Awake()
{
}

void buki::Player::Start()
{
	m_Entity->AddComponent<EditorController>()->SetCanStopTime(false);
	debugText = m_Entity->GetComponent<Text>();
	if (debugText == nullptr)
	{
		debugText = m_Entity->AddComponent<Text>();
	}
	debugText->SetFixed(false);
	debugText->LoadText("./fonts/Kenney/Kenney Blocks.ttf", 24);
	UpdateDebugText();

	rigidbody = m_Entity->GetComponent<RigidBody>();
	if (rigidbody == nullptr)
	{
		rigidbody = m_Entity->AddComponent<RigidBody>();
	}
	rigidbody->motionLocks = { true };

	collider = m_Entity->GetComponent<Box>();
	if (collider == nullptr)
	{
		collider = m_Entity->AddComponent<Box>();
	}
	collider->fillDraw = true;
	collider->Collider.material.restitution = 0.0f;
	collider->Collider.material.friction = 1.0f;
	collider->Collider.Size = playerSize;

	playerAnim = m_Entity->GetComponent<Animation>();
	if (playerAnim == nullptr)
	{
		playerAnim = m_Entity->AddComponent<Animation>();
	}

	playerAnim->Init(0, 96, 96);
	playerAnim->AddClip("Idle", 0, 0, 10, 0.1f, "./assets/Samurai/IDLE.png");

	playerAnim->Init(0, 96, 96);
	playerAnim->AddClip("Walk", 0, 0, 12, 0.1f, "./assets/Samurai/WALK.png");

	playerAnim->Init(0, 96, 96);
	playerAnim->AddClip("Run", 0, 0, 16, 0.08f, "./assets/Samurai/RUN.png");

	playerAnim->Init(0, 96, 96);
	playerAnim->AddClip("JumpUp", 0, 0, 3, 0.1f, "./assets/Samurai/JUMP-START.png");

	playerAnim->Init(0, 96, 96);
	playerAnim->AddClip("JumpMid", 0, 0, 3, 0.1f, "./assets/Samurai/JUMP-TRANSITION.png");

	playerAnim->Init(0, 96, 96);
	playerAnim->AddClip("JumpDown", 0, 0, 3, 0.1f, "./assets/Samurai/JUMP-FALL.png");

	//playerAnim->Load("./assets/Samurai/IDLE.png");
	//playerAnim->Init(0, 96, 96);
	//playerAnim->AddClip("Dash", 0, 2, 16, 0.2f, "./assets/Samurai/DASH.png");

	states.emplace("Idle", new PlayerIdle());
	states.emplace("Walk", new PlayerWalk());
	states.emplace("Run", new PlayerRun());
	states.emplace("Jump", new PlayerJump());

	m_Entity->ActivatePhysics();
	SetState("Idle");
}

void buki::Player::Update(const float dt)
{
	UpdateDebugText();
	if (Input().IsKeyPressed(EKey::EKEY_P))
	{
		World().SetLoadScene("Menu");
	}
	if (currentState != nullptr)
	{
		currentState->Update(dt, this);
	}

}

void buki::Player::FixedUpdate(const float dt)
{
	Vector2 vel = Physics().GetVelocity(m_Entity);
	if (currentState != nullptr)
	{
		currentState->FixedUpdate(dt, this);
	}
	Vector2 newCampos;
	newCampos = Graphics().GetCamera()->position;
	newCampos.x = m_Entity->GetTransform()->GetPosition().x;
	Graphics().SetCameraPosition(newCampos);
}

void buki::Player::Destroy()
{
	for (auto& state : states)
	{
		if (state.second != nullptr)
		{
			delete state.second;
			state.second = nullptr;
		}
	}
}


void buki::Player::OnCollisionEnter(Entity* other)
{
	if (other == nullptr) return;
	if (other->GetLayer() == "Ground")
	{
		currentState->OnCollisionEnter("Ground", other, this);
	}
}

void buki::Player::OnCollisionExit(Entity* other)
{

}

void buki::Player::OnCollisionHit(Entity* other)
{

}
json buki::Player::Serialize()
{
	json doc;
	return doc;
}

void buki::Player::Deserialize(json _doc)
{
}

void buki::Player::Set()
{
}

void buki::Player::Jump(float _force)
{
	Physics().LinearImpulse(m_Entity, Vector2(0, -1) * _force, true);
}

void buki::Player::Move(Vector2 _dir, float _maxVel, float _speed)
{
	float velX = Physics().GetVelocity(m_Entity).x;
	if (abs(velX) < _maxVel)
	{
		Physics().SetForce(m_Entity, _dir * _speed, true);
	}
}

void buki::Player::UpdateDebugText()
{
	std::string debugStr;
	for (const auto& state : states)
	{
		if (state.second == currentState)
		{
			debugStr = state.first;
		}
	}
	debugText->SetText(debugStr);
	Vector2 textSize = debugText->GetSize();
	debugText->SetPosition(Vector2{ 0.0f, -(playerSize.y + textSize.y)});
}
