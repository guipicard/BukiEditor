#pragma once
#include "PlayerIdle.h"
#include "Animation.h"
#include "Player.h"

buki::PlayerIdle::PlayerIdle()
{
}

void buki::PlayerIdle::Start(Player* player)
{
	player->GetEntity()->GetComponent<Animation>()->Play("Idle", true);
}

void buki::PlayerIdle::Update(float dt, Player* player)
{
	
	if (Input().IsKeyPressed(EKey::EKEY_A) ^ Input().IsKeyPressed(EKey::EKEY_D))
	{
		player->SetState("Walk");
	}
	if (Input().IsKeyPressed(EKey::EKEY_SPACE))
	{
		player->SetState("Jump");
	}
}

void buki::PlayerIdle::FixedUpdate(float dt, Player* player)
{
}

void buki::PlayerIdle::Exit(Player* player)
{
}

void buki::PlayerIdle::OnCollisionEnter(std::string value, Entity* other, Player* player)
{
}
