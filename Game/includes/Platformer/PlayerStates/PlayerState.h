#pragma once
#include <string>
#include "IInput.h"
#include "ILogger.h"
#include "IGraphics.h"
#include "IAudio.h"
#include "ICollision.h"
#include "IWorld.h"
#include "PhysicsService.h"

namespace buki
{
	struct Player;
	struct Entity;
	struct PlayerState
	{
		virtual ~PlayerState() = default;
		virtual void Start(Player* player) = 0;
		virtual void Update(float dt, Player* player) = 0;
		virtual void FixedUpdate(float dt, Player* player) = 0;
		virtual void Exit(Player* player) = 0;
		virtual void OnCollisionEnter(std::string value, Entity* other, Player* player) = 0;

	protected:
		IInput& Input() const;
		ILogger& Log() const;
		IGraphics& Graphics() const;
		IAudio& Audio() const;
		ICollision& Collision() const;
		IWorld& World() const;
		PhysicsService& Physics() const;
	};
}