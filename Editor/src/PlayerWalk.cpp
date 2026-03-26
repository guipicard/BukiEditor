#pragma once
#include "PlayerWalk.h"
#include "Animation.h"
#include "Player.h"

buki::PlayerWalk::PlayerWalk()
{
	speed = 50.0f;
	maxVel = 5.0f;
}

void buki::PlayerWalk::Start(Player* player)
{
	player->GetEntity()->GetComponent<Animation>()->Play("Walk", true);
}

void buki::PlayerWalk::Update(float dt, Player* player)
{
	if (Input().IsKeyPressed(EKey::EKEY_LEFTSHIFT))
	{
		player->SetState("Run");
	}
	if (!Input().IsKeyPressed(EKey::EKEY_A) && !Input().IsKeyPressed(EKey::EKEY_D))
	{
		player->SetState("Idle");
	}
	if (Input().IsKeyPressed(EKey::EKEY_SPACE))
	{
		player->SetState("Jump");
	}
}

void buki::PlayerWalk::FixedUpdate(float dt, Player* player)
{
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
}

void buki::PlayerWalk::Exit(Player* player)
{
}

void buki::PlayerWalk::OnCollisionEnter(std::string value, Entity* other, Player* player)
{
}
