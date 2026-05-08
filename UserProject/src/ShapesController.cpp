#pragma once
#include "ShapesController.h"
#include "Engine.h"
#include "Prefabs.h"
#include "Sprite.h"
#include "RigidBody.h"
#include "Spawner.h"
#include "Text.h"
#include "Camera2D.h"
#include "Box.h"
#include "Polygon.h"
#include "Circle.h"
#include "Button.h"
//#include "ComponentRegistration.h"
//
//REGISTER_COMPONENT(ShapesController, "ShapesController");

buki::ShapesController::ShapesController(Entity* entity)
	: MonoBehaviour(entity)
{
}

void buki::ShapesController::Start()
{

	for (auto* e : World().GetEntitiesInWorld())
	{
		if (e->GetComponent<Button>())
		{
			buttonsInScene.push_back(e->GetComponent<Button>());
		}
	}
}

void buki::ShapesController::Destroy()
{
}

void buki::ShapesController::FixedUpdate(const float dt)
{
}

void buki::ShapesController::Update(const float dt)
{
	UIHovered = false;
	for (auto* button : buttonsInScene)
	{
		if (button->IsHovered())
		{
			UIHovered = true;
			break;
		}
	}
	if (Input().IsKeyDown(EKey::EKEY_BACKSPACE))
	{
		World().SetLoadScene("Menu");
	}
	Vector2 mousePos;
	Input().GetMousePositionWorld(&mousePos.x, &mousePos.y);
	Vector2 pos = mousePos;
	float timeScale = Engine::Get().GetTimeScale();

	

	if (!UIHovered && timeScale != 0.0f)
	{
		if (Input().IsMouseButtonUp(0))
		{
			int sizeX = rand() % 10 + 2;
			int sizeY = rand() % 10 + 2;
			float rot = static_cast<float>((rand() % 7000) / 1000.0f);
			Vector2 randomSize = Vector2(sizeX, sizeY) / 5;

			Entity* e = World().InstantiatePrefab(boxRef.prefabPath);
			
			Box* s = e->GetComponent<Box>();
			s->def.fillDraw = boxFillDraw;
			s->def.shapeDraw = boxShapeDraw;
			s->def.size = randomSize;

			e->Initialize(pos, rot, randomSize);
			e->Set();
		}
		if (Input().IsMouseButtonUp(1))
		{
			int radius = rand() % 6 + 2;
			int polygonSides = rand() % 6 + 3;
			float r = (float)radius;
			r /= 4;
			float rot = static_cast<float>((rand() % 7000) / 1000.0f);
			Vector2 randomSize = Vector2(r, r);

			Entity* e = World().InstantiatePrefab(polygonRef.prefabPath);
			Polygon* s = e->GetComponent<Polygon>();
			s->def.fillDraw = boxFillDraw;
			s->def.shapeDraw = boxShapeDraw;
			s->def.radius = r;
			s->def.segments = polygonSides;

			e->Initialize(pos, rot, randomSize);
			e->Set();
		}
		if (Input().IsMouseButtonUp(2))
		{
			int radius = rand() % 6 + 2;
			float r = (float)radius;
			r /= 4;
			float rot = static_cast<float>((rand() % 7000) / 1000.0f);
			Vector2 randomSize = Vector2(r, r);

			Entity* e = World().InstantiatePrefab(circleRef.prefabPath);
			Circle* s = e->GetComponent<Circle>();
			s->def.fillDraw = circleFillDraw;
			s->def.shapeDraw = circleShapeDraw;
			s->def.radius = r;

			e->Initialize(pos, rot, randomSize);
			e->Set();
		}
	}

	for (auto e : World().GetEntitiesInWorld())
	{
		if (e == nullptr) return;
		Transform* tm = e->T();
		Vector2 pos = tm->GetPosition();
		if (pos.y > 30.0f)
		{
			World().Remove(e);
		}
	}
}


void buki::ShapesController::OnCollisionEnter(Entity* other)
{
}

void buki::ShapesController::OnCollisionExit(Entity* other)
{
}

void buki::ShapesController::OnCollisionHit(Entity* other)
{
}

void buki::ShapesController::OnSensorEnter(Entity* other)
{
}

void buki::ShapesController::OnSensorExit(Entity* other)
{
}

void buki::ShapesController::OnSet()
{
}
