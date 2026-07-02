#pragma once
#include "CameraFollow.h"
#include "Entity.h"
#include "Engine.h"

void buki::CameraFollow::Update(float dt)
{
}

void buki::CameraFollow::FixedUpdate(float dt) 
{
	if (target.Empty()) return;
	Entity* targetEntity = ResolveEntityRef(target);
	if (targetEntity != nullptr)
	{
		Camera2D* camera = buki::Engine::Get().GetActiveCameraPtr();
		Vector2 targetPos = targetEntity->Tm().GetPosition();
		camera->position.x = targetPos.x;
		camera->position.y = targetPos.y;
	}

}

buki::CameraFollow::CameraFollow(Entity* entity) : MonoBehaviour(entity) {}

void buki::CameraFollow::Start() {}
void buki::CameraFollow::Destroy() {}
void buki::CameraFollow::OnCollisionEnter(Entity* other) {}
void buki::CameraFollow::OnCollisionExit(Entity* other) {}
void buki::CameraFollow::OnCollisionHit(Entity* other) {}
void buki::CameraFollow::OnSensorEnter(Entity* other) {}
void buki::CameraFollow::OnSensorExit(Entity* other) {}
void buki::CameraFollow::OnSet() {}
