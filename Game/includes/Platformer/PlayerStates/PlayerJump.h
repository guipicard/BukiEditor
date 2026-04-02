#pragma once
#include "PlayerState.h"

namespace buki
{
	struct Player;
	struct Entity;
	struct PlayerJump : public PlayerState
	{
		PlayerJump();
		virtual void Start(Player* player) override;
		virtual void Update(float dt, Player* player) override;
		virtual void FixedUpdate(float dt, Player* player) override;
		virtual void Exit(Player* player) override;
		virtual void OnCollisionEnter(std::string value, Entity* other, Player* player) override;
	private:
		float speed;
		float maxVel;
		float jumpForce;
		float jumpVel;
		float transitionThreshold;
	};
}