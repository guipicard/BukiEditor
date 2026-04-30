#pragma once
#include "Polygon.h"

#include "box2d.h"
#include "Entity.h"
#include "Engine.h"
#include "RigidBody.h"
#include "BukiContainers.h"
#include "Units.h"
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>

void buki::Polygon::Draw(float alpha)
{
    const Vector2 pos = m_Entity->T()->GetPosition();
    const float angle = m_Entity->T()->GetRotation().GetRadians();

    if (def.fillDraw)
    {
        Graphics().FillPolygon(pos, def.radius, angle, def.fillColor, def.segments);
    }

    if (def.shapeDraw)
    {
        Graphics().DrawPolygonOutline(pos, def.radius, angle, def.shapeColor, def.segments);
    }
}

void buki::Polygon::Start()
{
}

void buki::Polygon::Destroy()
{
}

void buki::Polygon::SetPhysics()
{
    RigidBody* rb = m_Entity->GetComponent<RigidBody>();
    if (!rb)
    {
        return;
    }

    if (def.segments < 3)
    {
        def.segments = 3;
		Log().LogMessage("Polygon segments must be greater than or equal to 3. Defaulting to 3.");
    }
    if (def.radius <= 0.0f)
    {
        def.radius = 1.0f;
		Log().LogMessage("Polygon radius must be greater than 0. Defaulting to 1.0f.");
	}
    if (def.segments > 8)
    {
        def.segments = 8;
		Log().LogMessage("Polygon segments must be less than or equal to 8. Defaulting to 8.");
	}
	Transform* t = m_Entity->T();
    glm::vec2 center;
    center.x = 0.0f;
    center.y = 0.0f;
    std::vector<glm::vec2> verts;
	verts.reserve(def.segments);
    verts = Graphics().GetPolygonPoints(center, def.radius, 0.0f, def.segments, false);
    std::vector<b2Vec2> points;
    points.reserve(verts.size());
    for (const auto& v : verts)
    {
        points.push_back({ v.x, v.y });
    }
	b2Hull hull = b2ComputeHull(points.data(), static_cast<int>(points.size()));
    b2Polygon polygon = b2MakePolygon(&hull, 0);

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

    b2ShapeId s2Id = b2CreatePolygonShape(b2Id, &shapeDef, &polygon);
    ShapeId sId = { s2Id.index1, s2Id.world0, s2Id.generation };

    SetShapeId(sId);
    Physics().Listen(m_Entity);
}

json buki::Polygon::Serialize()
{
    json doc = SerializeShapeDef(def);
    doc["type"] = "Polygon";
    doc["polygon"]["radius"] = def.radius;
    doc["polygon"]["segments"] = def.segments;
    return doc;
}

void buki::Polygon::Deserialize(json _doc)
{
    def = DefaultPolygonShapeDef();
    DeserializeShapeDef(_doc, def);

    if (_doc.contains("polygon"))
    {
        def.radius = _doc["polygon"].value("radius", def.radius);
        def.segments = _doc["polygon"].value("segments", def.segments);
    }
}

void buki::Polygon::Set()
{
    if (!m_Entity->GetComponent<RigidBody>())
    {
        return;
    }
    m_Entity->ActivatePhysics();
}