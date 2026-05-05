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
	spawner = m_Entity->GetComponent<Spawner>();
	if (spawner == nullptr)
	{
		spawner = m_Entity->AddComponent<Spawner>();
	}
	spawner->AddPrototype("circle", new CircleShape());
	spawner->AddPrototype("rectangle", new RectangleShape());

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
	Vector2 size = Vector2();
	float timeScale = Engine::Get().GetTimeScale();
	if (!UIHovered && timeScale != 0.0f)
	{
		if (Input().IsMouseButtonUp(0))
		{

			Entity* e = spawner->Spawn("rectangle", pos, size, 0.0f);
			Box* s = e->GetComponent<Box>();
			//if (s)
			{
				s->def.fillDraw = boxFillDraw;
				s->def.shapeDraw = boxShapeDraw;
			}
			Vector2 colSize = s->def.size;
		}
		if (Input().IsMouseButtonUp(2))
		{
			Entity* e = spawner->Spawn("circle", pos, size, 0.0f);
			Circle* s = e->GetComponent<Circle>();
			//if (s)
			{
				s->def.fillDraw = circleFillDraw;
				s->def.shapeDraw = circleShapeDraw;
			}
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

void buki::ShapesController::Set()
{
}
