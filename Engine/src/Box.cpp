#pragma once
#include "Box.h"
#include "box2d.h"
#include "Entity.h"
#include "Engine.h"
#include "RigidBody.h"
#include "BukiContainers.h"

void buki::Box::Draw(float alpha)
{
	int r = static_cast<int>(ShapeColor.r * 255);
	int g = static_cast<int>(ShapeColor.g * 255);
	int b = static_cast<int>(ShapeColor.b * 255);
	int a = static_cast<int>(ShapeColor.a * 255);
	int pix[16] = {
		r, g, b, a,
		r, g, b, a,
		r, g, b, a,
		r, g, b, a
	};
	Vector2 pos = m_Entity->GetTransform()->GetPosition();
	Vector2 size = m_Entity->GetTransform()->GetSize();
	float angle = m_Entity->GetTransform()->GetRotation().GetRadians();
	if (fillDraw)
	{
		//Graphics().FillRect(pos.x, pos.y, Collider.Size.x, Collider.Size.y, angle, ShapeColor);
		uint32_t debugTexture = Graphics().CreateTextureRGBA8(pix, 4, 4);
		Graphics().DrawTexturedQuad(debugTexture, 
			pos.x, pos.y, size.x, size.y,
			angle, 0.0f, 0.0f, 
			0.0f, 0.0f, 1.0f, 1.0f, 
			1.0f, 1.0f, 1.0f, 1.0f);
	}
	if (shapeDraw)
	{
		//Graphics().DrawRect(pos.x, pos.y, Collider.Size.x, Collider.Size.y, angle, DebugColor);
		uint32_t debugTexture = Graphics().CreateTextureRGBA8(pix, 4, 4);
		Graphics().DrawTexturedQuad(debugTexture, 
			pos.x, pos.y, size.x, size.y, 
			angle, 0.0f, 0.0f, 
			0.0f, 0.0f, 1.0f, 1.0f, 
			1.0f, 1.0f, 1.0f, 1.0f);
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
	def.density = Collider.density;
	def.material.friction = Collider.material.friction;
	def.material.restitution = Collider.material.restitution;

	def.enableContactEvents = true;
	def.userData = m_Entity; // Set user data to the entity pointer
	def.isSensor = Sensor;
	def.filter.categoryBits = filter;

	Vector2 size = Collider.Size;
	BodyId bId = rb->GetBodyId();
	b2BodyId b2Id = b2BodyId{ bId.index1, bId.world0, bId.generation };
	b2Polygon box = b2MakeBox(size.x, size.y);
	b2ShapeId s2Id = b2CreatePolygonShape(b2Id, &def, &box);
	ShapeId sId = { s2Id.index1, s2Id.world0, s2Id.generation };
	SetShapeId(sId);
	Physics().AddShape(sId.index1, m_Entity);

	Physics().Listen(m_Entity); // Set user data to the entity pointer
}

json buki::Box::Serialize()
{
	json doc = Shapes::Serialize();
	doc["type"] = "Box";
	doc["BoxCollider"]["size"]["x"] = Collider.Size.x;
	doc["BoxCollider"]["size"]["y"] = Collider.Size.y;
	doc["BoxCollider"]["CanDraw"] = Collider.m_CanDraw;
	doc["BoxCollider"]["density"] = Collider.density;
	doc["BoxCollider"]["friction"] = Collider.material.friction;
	doc["BoxCollider"]["restitution"] = Collider.material.restitution;
	return doc;
}

void buki::Box::Deserialize(json _doc)
{
	Shapes::Deserialize(_doc);
	Collider.Size.x = _doc["BoxCollider"]["size"]["x"].get<float>();
	Collider.Size.y = _doc["BoxCollider"]["size"]["y"].get<float>();
	Collider.density = _doc["BoxCollider"]["density"].get<float>();
	Collider.material.friction = _doc["BoxCollider"]["friction"].get<float>();
	Collider.material.restitution = _doc["BoxCollider"]["restitution"].get<float>();
}

void buki::Box::Set()
{

}
