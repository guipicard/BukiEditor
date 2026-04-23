#include "Box.h"

#include "box2d.h"
#include "Entity.h"
#include "Engine.h"
#include "RigidBody.h"
#include "BukiContainers.h"
#include "Units.h"


void buki::Box::Draw(float alpha)
{

	Vector2 pos = m_Entity->T()->GetPosition();
	float angle = m_Entity->T()->GetRotation().GetRadians();
	RectF dstRect = { pos.x, pos.y, def.size.x, def.size.y };

	if (def.fillDraw)
	{
		Graphics().FillRect(pos, def.size, angle, def.fillColor);
	}

	if (def.shapeDraw)
	{
		Graphics().DrawRectOutline(pos, def.size, angle, def.shapeColor);
	}
}

void buki::Box::Start()
{
}

void buki::Box::Destroy()
{
}

void buki::Box::SetPhysics()
{
	RigidBody* rb = m_Entity->GetComponent<RigidBody>();
	if (!rb)
	{
		return;
	}

	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = def.density;
	shapeDef.material.friction = def.friction;
	shapeDef.material.restitution = def.restitution;
	shapeDef.enableContactEvents = true;
	shapeDef.userData = m_Entity;
	shapeDef.isSensor = def.isSensor;
	//shapeDef.filter.categoryBits = def.filter;

	BodyId bId = rb->GetBodyId();
	b2BodyId b2Id = b2BodyId{ bId.index1, bId.world0, bId.generation };
	b2Polygon box = b2MakeBox(def.size.x / 2.0f, def.size.y / 2.0f);
	b2ShapeId s2Id = b2CreatePolygonShape(b2Id, &shapeDef, &box);
	ShapeId sId = { s2Id.index1, s2Id.world0, s2Id.generation };

	SetShapeId(sId);
	Physics().Listen(m_Entity);
}

json buki::Box::Serialize()
{
	json doc = Shapes::Serialize();
	doc["type"] = "Box";
	doc["box"]["size"]["x"] = def.size.x;
	doc["box"]["size"]["y"] = def.size.y;
	return doc;
}

void buki::Box::Deserialize(json _doc)
{
	Shapes::Deserialize(_doc);

	if (_doc.contains("box"))
	{
		def.size.x = _doc["box"]["size"].value("x", 1.0f);
		def.size.y = _doc["box"]["size"].value("y", 1.0f);
	}
}

void buki::Box::Set()
{
}
