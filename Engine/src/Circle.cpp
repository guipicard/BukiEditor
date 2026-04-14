#pragma once
#include "Circle.h"
#include "box2d.h"
#include "Entity.h"
#include "RigidBody.h"
#include "IGraphics.h"
#include "ILogger.h"

void buki::Circle::Draw(float alpha)
{
	if (fillDraw)
	{
		Vector2 pos = m_Entity->GetTransform()->GetPosition();
		float angle = m_Entity->GetTransform()->GetRotation().GetRadians();
		//Graphics().FillCircle(pos.x, pos.y, Collider.Radius, angle, ShapeColor);
	}
	if (shapeDraw)
	{
		Vector2 pos = m_Entity->GetTransform()->GetPosition();
		float angle = m_Entity->GetTransform()->GetRotation().GetRadians();
		//Graphics().DrawCircle(pos.x, pos.y, Collider.Radius, angle, DebugColor);
	}
}

void buki::Circle::Start()
{
}

void buki::Circle::Destroy()
{
}

void buki::Circle::SetPhysics()
{
	if (!m_Entity->GetComponent<RigidBody>())
	{
		m_Entity->AddComponent<RigidBody>();
		m_Entity->ActivatePhysics();
	}
	else if (!m_Entity->HasPhysics())
	{
		m_Entity->ActivatePhysics();
	}
	b2ShapeDef def = b2DefaultShapeDef();
	def.density = Collider.density;
	def.material.friction = Collider.material.friction;
	def.material.restitution = Collider.material.restitution;

	def.enableContactEvents = true;
	def.userData = m_Entity; // Set user data to the entity pointer

	BodyId bId = m_Entity->GetComponent<RigidBody>()->GetBodyId();
	b2BodyId b2Id = b2BodyId{ bId.index1, bId.world0, bId.generation };
	b2Circle circle;
	circle.center = { Collider.PositionOffset.x, Collider.PositionOffset.y };
	circle.radius = Collider.Radius;
	b2ShapeId s2Id = b2CreateCircleShape(b2Id, &def, &circle);
	ShapeId sId = { s2Id.index1, s2Id.world0, s2Id.generation };
	m_Entity->GetComponentOfType<Shapes>()->SetShapeId(sId);

	Physics().AddShape(sId.index1, m_Entity);
}

json buki::Circle::Serialize()
{
	json doc = Shapes::Serialize();
	doc["Collider"]["Radius"] = Collider.Radius;
	doc["Collider"]["CanDraw"] = Collider.m_CanDraw;
	doc["Collider"]["Density"] = Collider.density;
	doc["Collider"]["Friction"] = Collider.material.friction;
	doc["Collider"]["Restitution"] = Collider.material.restitution;
	doc["Collider"]["PositionOffset"]["x"] = Collider.PositionOffset.x;
	doc["Collider"]["PositionOffset"]["y"] = Collider.PositionOffset.y;
	return doc;
}

void buki::Circle::Deserialize(json _doc)
{
	Shapes::Deserialize(_doc);
	Collider.Radius = _doc["Collider"]["Radius"].get<float>();
	Collider.m_CanDraw = _doc["Collider"]["CanDraw"].get<bool>();
	Collider.density = _doc["Collider"]["Density"].get<float>();
	Collider.material.friction = _doc["Collider"]["Friction"].get<float>();
	Collider.material.restitution = _doc["Collider"]["Restitution"].get<float>();
	Collider.PositionOffset.x = _doc["Collider"]["PositionOffset"]["x"].get<float>();
	Collider.PositionOffset.y = _doc["Collider"]["PositionOffset"]["y"].get<float>();
}

void buki::Circle::Set()
{
}
