#pragma once
#include "PlayerState.h"

namespace buki
{
	struct Player;
	struct Entity;
	struct PlayerIdle : public PlayerState
	{
		PlayerIdle();
		virtual void Start(Player* player) override;
		virtual void Update(float dt, Player* player) override;
		virtual void FixedUpdate(float dt, Player* player) override;
		virtual void Exit(Player* player) override;
		virtual void OnCollisionEnter(std::string value, Entity* other, Player* player) override;
	};
}
