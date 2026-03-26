#pragma once
#include "ShapesController.h"
#include "Engine.h"
#include "Prefabs.h"
#include "Sprite.h"
#include "CircleCollider.h"
#include "BoxCollider.h"
#include "RigidBody.h"
#include "Spawner.h"
#include "Text.h"
#include "Camera.h"
#include "Box.h"
#include "Circle.h"
#include "EditorController.h"
#include "ComponentRegistration.h"

REGISTER_COMPONENT(ShapesController, "ShapesController");

buki::ShapesController::ShapesController(Entity* entity)
	: MonoBehaviour(entity)
{
	Awake();
}

void buki::ShapesController::Awake()
{
	spawner = nullptr;
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
#if _DEBUG
	m_Entity->AddComponent<EditorController>();
#endif
}

void buki::ShapesController::FixedUpdate(const float dt)
{
}

void buki::ShapesController::Update(const float dt)
{
	if (Input().IsKeyDown(EKey::EKEY_BACKSPACE))
	{
		World().SetLoadScene("Menu");
	}
	float mouseX, mouseY;
	Input().GetMousePosition(&mouseX, &mouseY);
	Vector2 pos = Vector2(mouseX, mouseY);
	Vector2 size = Vector2();
	std::vector<Entity*> hitEntities;
	bool hit = Physics().QueryPoint(pos, hitEntities, 1 << 8);
	if (hit)
	{
		float timeScale = Engine::GetInstance().GetTimeScale();
		if (hitEntities[0] && timeScale == 1.0f)
		{
			if (!hitEntities[0]->GetComponent<Button>())
			{
				hit = false;
			}
		}
	}
	if (!hit)
	{

		if (Input().IsButtonUp(0))
		{

			Entity* e = spawner->Spawn("rectangle", pos, size, 0.0f);
			Box* s = e->GetComponent<Box>();
			//if (s)
			{
				s->fillDraw = boxFillDraw;
				s->shapeDraw = boxShapeDraw;
			}
			Vector2 colSize = s->Collider.Size;
		}
		if (Input().IsButtonUp(2))
		{
			Entity* e = spawner->Spawn("circle", pos, size, 0.0f);
			Circle* s = e->GetComponent<Circle>();
			//if (s)
			{
				s->fillDraw = circleFillDraw;
				s->shapeDraw = circleShapeDraw;
			}
		}
	}

	for (auto e : World().GetEntitiesInWorld())
	{
		if (e == nullptr) return;
		Transform* tm = e->GetTransform();
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

json buki::ShapesController::Serialize()
{
	json doc;
	doc["circleFillDraw"] = circleFillDraw;
	doc["circleShapeDraw"] = circleShapeDraw;
	doc["boxFillDraw"] = boxFillDraw;
	doc["boxShapeDraw"] = boxShapeDraw;
	return doc;
}

void buki::ShapesController::Deserialize(json _doc)
{
	circleFillDraw = _doc["circleFillDraw"].get<bool>();
	circleShapeDraw = _doc["circleShapeDraw"].get<bool>();
	boxFillDraw = _doc["boxFillDraw"].get<bool>();
	boxShapeDraw = _doc["boxShapeDraw"].get<bool>();
}

void buki::ShapesController::Set()
{
}
