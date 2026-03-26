#pragma once
#include "Box.h"
#include "box2d.h"
#include "Entity.h"
#include "RigidBody.h"
#include "IGraphics.h"
#include "ILogger.h"

void buki::Box::Draw(float alpha)
{
	if (fillDraw)
	{
		Vector2 pos = m_Entity->GetTransform()->GetPosition();
		float angle = m_Entity->GetTransform()->GetRotation().GetRadians();
		Graphics().FillRect(pos.x, pos.y, Collider.Size.x, Collider.Size.y, angle, ShapeColor);
	}
	if (shapeDraw)
	{
		Vector2 pos = m_Entity->GetTransform()->GetPosition();
		float angle = m_Entity->GetTransform()->GetRotation().GetRadians();
		Graphics().DrawRect(pos.x, pos.y, Collider.Size.x, Collider.Size.y, angle, DebugColor);
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
	b2ShapeDef def = b2DefaultShapeDef();
	def.density = Collider.Density;
	def.friction = Collider.Friction;
	def.restitution = Collider.Restitution;

	def.enableContactEvents = true;
	def.userData = m_Entity; // Set user data to the entity pointer
	def.isSensor = Sensor;
	def.filter.categoryBits = filter;

	Vector2 size = Collider.Size;
	BodyId bId = rb->GetBodyId();
	b2BodyId b2Id = b2BodyId{ bId.index1, bId.world0, bId.revision };
	b2Polygon box = b2MakeBox(size.x, size.y);
	b2ShapeId s2Id = b2CreatePolygonShape(b2Id, &def, &box);
	ShapeId sId = { s2Id.index1, s2Id.world0, s2Id.revision };
	SetShapeId(sId);
	Physics().AddShape(sId.index1,m_Entity);

	Physics().Listen(m_Entity); // Set user data to the entity pointer
}

json buki::Box::Serialize()
{
	json doc = Shapes::Serialize();
	doc["type"] = "Box";
	doc["BoxCollider"]["size"]["x"] = Collider.Size.x;
	doc["BoxCollider"]["size"]["y"] = Collider.Size.y;
	doc["BoxCollider"]["CanDraw"] = Collider.m_CanDraw;
	doc["BoxCollider"]["density"] = Collider.Density;
	doc["BoxCollider"]["friction"] = Collider.Friction;
	doc["BoxCollider"]["restitution"] = Collider.Restitution;
	return doc;
}

void buki::Box::Deserialize(json _doc)
{
	Shapes::Deserialize(_doc);
	json j = _doc["BoxCollider"];
	//Collider.Size.x = _doc["BoxCollider"]["size"]["x"].get<float>();
	//Collider.Size.y = _doc["BoxCollider"]["size"]["y"].get<float>();
	//Collider.Density = _doc["BoxCollider"]["density"].get<float>();
	//Collider.Friction = _doc["BoxCollider"]["friction"].get<float>();
	//Collider.Restitution = _doc["BoxCollider"]["restitution"].get<float>();
	json sizeJson = j["size"];
	Collider.Size.x = sizeJson.value<float>("x", Collider.Size.x);
	Collider.Size.y = sizeJson.value<float>("y", Collider.Size.y);
	Collider.Density = j.value<float>("density", Collider.Density);
	Collider.Friction = j.value<float>("friction", Collider.Friction);
	Collider.Restitution = j.value<float>("restitution", Collider.Restitution);
}

void buki::Box::Set()
{

}
