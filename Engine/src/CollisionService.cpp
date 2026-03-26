#pragma once
#include "Engine.h"
#include "CollisionService.h"
#include "math.h"
#include "Entity.h"
#include "RigidBody.h"
#include "Collider.h"
#include "Vector2.h"
#include "RigidBody.h"
#include "RectF.h"
#include "CircleCollider.h"
#include "Transform.h"
#include "Camera.h"
#include <cstdlib>
#include <cfloat>
#include <cassert>

bool buki::CollisionService::CheckPointRect(float px, float py, float rx, float ry, float rw, float rh)
{
	float scale = METRES_TO_PIXELS;
	Camera* camera = buki::Engine::GetInstance().Graphics().GetCamera();
	Vector2 camPos = camera->position * scale;
	Vector2 camSize = Vector2(camera->width, camera->height);

	float _px = px * scale;
	float _py = py * scale;
	_px -= camPos.x - (camSize.x * 0.5f);
	_py -= camPos.y - (camSize.y * 0.5f);

	RectF _rect;
	
	_rect.x = (rx - (rw / 2)) * scale;
	_rect.y = (ry - (rh / 2)) * scale;
	_rect.w = rw * scale;
	_rect.h = rh * scale;

	_rect.x -= camPos.x - (camSize.x * 0.5f);
	_rect.y -= camPos.y - (camSize.y * 0.5f);

	return _px >= _rect.x && _py >= _rect.y && _px <= (_rect.x + _rect.w) && _py <= (_rect.y + _rect.h);
}

bool buki::CollisionService::CheckPointCircle(float px, float py, float cx, float cy, float cr)
{
	float vecX = px - cx;
	float vecY = py - cy;
	float d = sqrtf((vecX * vecX) + (vecY * vecY));

	return d <= cr;
}

void buki::CollisionService::CheckCircles(Manifold* M)
{
}

void buki::CollisionService::CheckRects(Manifold* M)
{
}

void buki::CollisionService::CheckCircleRect(Manifold* M, Entity* a, Entity* b)
{
}

void buki::CollisionService::CheckRectCircle(Manifold* M, Entity* a, Entity* b)
{
}

bool buki::CollisionService::CheckAABB(AABB A, AABB B)
{
	return true;
}
