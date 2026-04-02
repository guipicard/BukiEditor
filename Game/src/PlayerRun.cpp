#pragma once
#include "PlayerRun.h"
#include "Animation.h"
#include "Player.h"

buki::PlayerRun::PlayerRun()
{
	maxVel = 10.0f;
	speed = 50.0f;
}

void buki::PlayerRun::Start(Player* player)
{
	player->GetEntity()->GetComponent<Animation>()->Play("Run", true);
}

void buki::PlayerRun::Update(float dt, Player* player)
{
	if (!Input().IsKeyPressed(EKey::EKEY_LEFTSHIFT))
	{
		player->SetState("Walk");
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

void buki::PlayerRun::FixedUpdate(float dt, Player* player)
{
	if (Input().IsKeyPressed(EKey::EKEY_A) && Input().IsKeyPressed(EKey::EKEY_D))
	{
		return;
	}
	if (Input().IsKeyPressed(EKey::EKEY_A))
	{
		player->Move({ -1.0f, 0.0f }, maxVel, speed);
		player->GetEntity()->GetComponent<Animation>()->SetFlip(true, false);
	}
	if (Input().IsKeyPressed(EKey::EKEY_D))
	{
		player->Move({ 1.0f, 0.0f }, maxVel, speed);
		player->GetEntity()->GetComponent<Animation>()->SetFlip(false, false);
	}
}

void buki::PlayerRun::Exit(Player* player)
{
}

void buki::PlayerRun::OnCollisionEnter(std::string value, Entity* other, Player* player)
{
}
