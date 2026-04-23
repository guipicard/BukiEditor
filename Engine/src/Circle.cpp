#pragma once
#include "Circle.h"
#include "box2d.h"
#include "Entity.h"
#include "RigidBody.h"
#include "IGraphics.h"
#include "ILogger.h"

void buki::Circle::Draw(float alpha)
{
	Vector2 pos = m_Entity->T()->GetPosition();

	if (def.fillDraw)
	{
		Graphics().FillCircle(pos, def.radius, def.fillColor);
	}

	if (def.shapeDraw)
	{
		Graphics().DrawCircleOutline(pos, def.radius, def.shapeColor);
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
	RigidBody* rb = m_Entity->GetComponent<RigidBody>();
	if (!rb)
	{
		return;
	}
	b2ShapeDef b2def = b2DefaultShapeDef();
	//b2def.density = def.density;
	b2def.material.friction = def.friction;
	b2def.material.restitution = def.restitution;
	b2def.enableContactEvents = true;
	b2def.userData = m_Entity;
	b2def.isSensor = def.isSensor;
	//b2def.filter.categoryBits = def.filter;

	BodyId bId = rb->GetBodyId();
	b2BodyId b2Id = b2BodyId{ bId.index1, bId.world0, bId.generation };

	b2Circle circle;
	circle.center = { 0.0f,0.0f };
	circle.radius = def.radius;

	b2ShapeId s2Id = b2CreateCircleShape(b2Id, &b2def, &circle);
	ShapeId sId = { s2Id.index1, s2Id.world0, s2Id.generation };

	SetShapeId(sId);
	Physics().Listen(m_Entity);
}

json buki::Circle::Serialize()
{
	json doc = Shapes::Serialize();
	doc["type"] = "Circle";
	doc["circle"]["radius"] = def.radius;
	return doc;
}

void buki::Circle::Deserialize(json _doc)
{
	Shapes::Deserialize(_doc);

	if (_doc.contains("circle"))
	{
		def.radius = _doc["circle"].value("radius", 0.5f);
	}
}

void buki::Circle::Set()
{
}
