#pragma once
#include "Component.h"
#include "Engine.h"

buki::Component::Component() : Component(nullptr)
{
}

buki::Component::Component(Entity* _entity) : m_Entity(_entity) 
{
}

buki::IInput& buki::Component::Input() const
{
	return Engine::Get().Input();
}

buki::ILogger& buki::Component::Log() const
{
	return Engine::Get().Log();
}

buki::IGraphics& buki::Component::Graphics() const
{
	return Engine::Get().Graphics();
}

buki::IAudio& buki::Component::Audio() const
{
	return Engine::Get().Audio();
}

buki::IWorld& buki::Component::World() const
{
	return Engine::Get().World();
}

buki::PhysicsService& buki::Component::Physics() const
{
	return Engine::Get().Physics();
}

buki::TextureManager& buki::Component::Textures() const
{
	return Engine::Get().Textures();
}

buki::Camera2D& buki::Component::Camera() const
{
	return Engine::Get().GetActiveCamera();
}
