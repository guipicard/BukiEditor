#pragma once
#include "PlayerJump.h"
#include "Animation.h"
#include "Player.h"
#include "Text.h"

buki::PlayerJump::PlayerJump()
{
	maxVel = 5.0f;
	speed = 25.0f;
	jumpForce = 25.0f;
	jumpVel = 0.0f;
	transitionThreshold = 0.3f;
}

void buki::PlayerJump::Start(Player* player)
{
	player->GetEntity()->GetComponent<Animation>()->Play("JumpUp", false);
	player->Jump(jumpForce);
	jumpVel = Physics().GetVelocity(player->GetEntity()).y;
	player->GetDebugText()->SetColor(Color::GREEN);
}

void buki::PlayerJump::Update(float dt, Player* player)
{
}

void buki::PlayerJump::FixedUpdate(float dt, Player* player)
{
	float currentVelY = Physics().GetVelocity(player->GetEntity()).y;
		Animation* anim = player->GetEntity()->GetComponent<Animation>();
	if (abs(currentVelY) < abs(jumpVel) * transitionThreshold)
	{
		if (currentVelY < 0)
		{
			anim->Play("JumpMid", false);
			player->GetDebugText()->SetColor(Color::RED);

		}
	}
	else
	{
		if (currentVelY > 0)
		{
			anim->Play("JumpDown", false);
			player->GetDebugText()->SetColor(Color::YELLOW);
		}
	}
	if (Input().IsKeyPressed(EKey::EKEY_A) && Input().IsKeyPressed(EKey::EKEY_D))
	{
		return;
	}
	if (Input().IsKeyPressed(EKey::EKEY_A))
	{
		player->Move(Vector2::LEFT, maxVel, speed);
		player->GetEntity()->GetComponent<Animation>()->SetFlip(true, false);
	}
	if (Input().IsKeyPressed(EKey::EKEY_D))
	{
		player->Move(Vector2::RIGHT, maxVel, speed);
		player->GetEntity()->GetComponent<Animation>()->SetFlip(false, false);
	}
	Vector2 vel = Physics().GetVelocity(player->GetEntity());
}

void buki::PlayerJump::Exit(Player* player)
{
	player->GetDebugText()->SetColor(Color::WHITE);
}

void buki::PlayerJump::OnCollisionEnter(std::string value, Entity* other, Player* player)
{
	if (value == "Ground")
	{
		player->SetState("Idle");
	}
}
