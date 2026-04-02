#pragma once
#include "PlayerState.h"
#include "Engine.h"

buki::IInput& buki::PlayerState::Input() const
{
	return Engine::GetInstance().Input();
}

buki::ILogger& buki::PlayerState::Log() const
{
	return Engine::GetInstance().Log();
}

buki::IGraphics& buki::PlayerState::Graphics() const
{
	return Engine::GetInstance().Graphics();
}

buki::IAudio& buki::PlayerState::Audio() const
{
	return Engine::GetInstance().Audio();
}

buki::IWorld& buki::PlayerState::World() const
{
	return Engine::GetInstance().World();
}

buki::PhysicsService& buki::PlayerState::Physics() const
{
	return Engine::GetInstance().Physics();
}
