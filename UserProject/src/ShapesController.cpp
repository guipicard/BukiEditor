#pragma once
#include "ShapesController.h"

#include "Box.h"
#include "Button.h"
#include "Circle.h"
#include "Engine.h"
#include "Entity.h"
#include "Polygon.h"

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

			Entity* e = World().InstantiatePrefab(boxRef.path);
			auto& t = e->Tm();
			
			Box* s = e->GetComponent<Box>();
			s->def.fillDraw = boxFillDraw;
			s->def.shapeDraw = boxShapeDraw;
			s->def.size = randomSize;

			Rot rotation = Rot();
			rotation.SetRadians(rot);
			t.UpdateState(pos, rotation, randomSize);

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

			Entity* e = World().InstantiatePrefab(polygonRef.path);
			auto& t = e->Tm();

			Polygon* s = e->GetComponent<Polygon>();
			s->def.fillDraw = boxFillDraw;
			s->def.shapeDraw = boxShapeDraw;
			s->def.radius = r;
			s->def.segments = polygonSides;

			Rot rotation = Rot();
			rotation.SetRadians(rot);
			t.UpdateState(pos, rotation, randomSize);
			e->Set();
		}
		if (Input().IsMouseButtonUp(2))
		{
			int radius = rand() % 6 + 2;
			float r = (float)radius;
			r /= 4;
			float rot = static_cast<float>((rand() % 7000) / 1000.0f);
			Vector2 randomSize = Vector2(r, r);

			Entity* e = World().InstantiatePrefab(circleRef.path);
			auto& t = e->Tm();

			Circle* s = e->GetComponent<Circle>();
			s->def.fillDraw = circleFillDraw;
			s->def.shapeDraw = circleShapeDraw;
			s->def.radius = r;

			Rot rotation = Rot();
			rotation.SetRadians(rot);
			t.UpdateState(pos, rotation, randomSize);
			e->Set();
		}
	}

	for (auto e : World().GetEntitiesInWorld())
	{
		if (e == nullptr) return;
		if (e->Tm().GetPosition().y > 30.0f)
		{
			World().Remove(e);
		}
	}
}

void buki::ShapesController::OnCollisionEnter(Entity* other){}
void buki::ShapesController::OnCollisionExit(Entity* other){}
void buki::ShapesController::OnCollisionHit(Entity* other){}
void buki::ShapesController::OnSensorEnter(Entity* other){}
void buki::ShapesController::OnSensorExit(Entity* other){}
void buki::ShapesController::OnSet(){}

void buki::ShapesController::DrawBoxFill()
{
	buki::ShapesController* controller = buki::Engine::Get().World().FindEntityByName("controller")->GetComponent<buki::ShapesController>();
	controller->SetDrawBoxFill(true);
	controller->SetDrawBoxShape(true);

	for (auto e : buki::Engine::Get().World().GetEntitiesInWorld())
	{
		buki::Box* box = e->GetComponent<buki::Box>();
		if (box && box->GetEntity()->GetLayer() == "Box")
		{
			box->def.fillDraw = true;
			box->def.shapeDraw = false;
		}
	}
}

void buki::ShapesController::DrawBoxShape()
{
	buki::ShapesController* controller = buki::Engine::Get().World().FindEntityByName("controller")->GetComponent<buki::ShapesController>();
	controller->SetDrawBoxFill(false);
	controller->SetDrawBoxShape(true);

	for (auto e : buki::Engine::Get().World().GetEntitiesInWorld())
	{
		buki::Box* box = e->GetComponent<buki::Box>();
		if (box && box->GetEntity()->GetLayer() == "Box")
		{
			box->def.fillDraw = false;
			box->def.shapeDraw = true;
		}
	}
}

void buki::ShapesController::DrawBoxBoth()
{
	buki::ShapesController* controller = buki::Engine::Get().World().FindEntityByName("controller")->GetComponent<buki::ShapesController>();
	controller->SetDrawBoxFill(false);
	controller->SetDrawBoxShape(true);

	for (auto e : buki::Engine::Get().World().GetEntitiesInWorld())
	{
		buki::Box* box = e->GetComponent<buki::Box>();
		if (box && box->GetEntity()->GetLayer() == "Box")
		{
			box->def.fillDraw = true;
			box->def.shapeDraw = true;
		}
	}
}

void buki::ShapesController::DrawCircleFill()
{
	buki::ShapesController* controller = buki::Engine::Get().World().FindEntityByName("controller")->GetComponent<buki::ShapesController>();
	controller->SetDrawCircleFill(false);
	controller->SetDrawCircleShape(true);

	for (auto e : buki::Engine::Get().World().GetEntitiesInWorld())
	{
		buki::Circle* circle = e->GetComponent<buki::Circle>();
		if (circle && circle->GetEntity()->GetLayer() == "Circle")
		{
			circle->def.fillDraw = true;
			circle->def.shapeDraw = false;
		}
	}
}

void buki::ShapesController::DrawCircleShape()
{
	buki::ShapesController* controller = buki::Engine::Get().World().FindEntityByName("controller")->GetComponent<buki::ShapesController>();
	controller->SetDrawCircleFill(false);
	controller->SetDrawCircleShape(true);

	for (auto e : buki::Engine::Get().World().GetEntitiesInWorld())
	{
		buki::Circle* circle = e->GetComponent<buki::Circle>();
		if (circle && circle->GetEntity()->GetLayer() == "Circle")
		{
			circle->def.fillDraw = false;
			circle->def.shapeDraw = true;
		}
	}
}

void buki::ShapesController::DrawCircleBoth()
{
	buki::ShapesController* controller = buki::Engine::Get().World().FindEntityByName("controller")->GetComponent<buki::ShapesController>();
	controller->SetDrawCircleFill(false);
	controller->SetDrawCircleShape(true);

	for (auto e : buki::Engine::Get().World().GetEntitiesInWorld())
	{
		buki::Circle* circle = e->GetComponent<buki::Circle>();
		if (circle && circle->GetEntity()->GetLayer() == "Circle")
		{
			circle->def.fillDraw = true;
			circle->def.shapeDraw = true;
		}
	}
}

void buki::ShapesController::DrawPolygonFill()
{
	buki::ShapesController* controller = buki::Engine::Get().World().FindEntityByName("controller")->GetComponent<buki::ShapesController>();
	controller->SetDrawPolygonFill(false);
	controller->SetDrawPolygonShape(true);

	for (auto e : buki::Engine::Get().World().GetEntitiesInWorld())
	{
		buki::Polygon* polygon = e->GetComponent<buki::Polygon>();
		if (polygon && polygon->GetEntity()->GetLayer() == "Polygon")
		{
			polygon->def.fillDraw = true;
			polygon->def.shapeDraw = false;
		}
	}
}

void buki::ShapesController::DrawPolygonShape()
{
	buki::ShapesController* controller = buki::Engine::Get().World().FindEntityByName("controller")->GetComponent<buki::ShapesController>();
	controller->SetDrawPolygonFill(false);
	controller->SetDrawPolygonShape(true);

	for (auto e : buki::Engine::Get().World().GetEntitiesInWorld())
	{
		buki::Polygon* polygon = e->GetComponent<buki::Polygon>();
		if (polygon && polygon->GetEntity()->GetLayer() == "Polygon")
		{
			polygon->def.fillDraw = false;
			polygon->def.shapeDraw = true;
		}
	}
}

void buki::ShapesController::DrawPolygonBoth()
{
	buki::ShapesController* controller = buki::Engine::Get().World().FindEntityByName("controller")->GetComponent<buki::ShapesController>();
	controller->SetDrawPolygonFill(false);
	controller->SetDrawPolygonShape(true);

	for (auto e : buki::Engine::Get().World().GetEntitiesInWorld())
	{
		buki::Polygon* polygon = e->GetComponent<buki::Polygon>();
		if (polygon && polygon->GetEntity()->GetLayer() == "Polygon")
		{
			polygon->def.fillDraw = true;
			polygon->def.shapeDraw = true;
		}
	}
}
