#pragma once
#include "Polygon.h"

#include "box2d.h"
#include "Entity.h"
#include "Engine.h"
#include "RigidBody.h"
#include "BukiContainers.h"
#include "Units.h"
#include "PolygonGeometry.h"
#include <cmath>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>

void buki::Polygon::Draw(float alpha)
{
	const auto& t = m_Entity->Tm();
    const Vector2 pos = t.GetPosition();
    const float angle = t.GetRotation().GetRadians();

    if (def.fillDraw)
    {
        Graphics().FillPolygon(pos, def.radius, angle, def.fillColor, def.segments);
    }

    if (def.shapeDraw)
    {
        const LineStyle lineStyle{ def.shapeThickness, LineThicknessSpace::ScreenPixels };
        Graphics().DrawPolygonOutline(pos, def.radius, angle, def.shapeColor, def.segments, lineStyle);
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
    if (m_Entity == nullptr)
    {
        Log().LogError("Cannot set physics on polygon because entity has no rigidbody.");
        return;
    }

    RigidBody* rigidBody = m_Entity->GetComponent<RigidBody>();

    if (rigidBody == nullptr)
    {
        return;
    }

    if (def.segments < 3)
    {
        def.segments = 3;
        Log().LogError("Polygon segments must be greater than or equal to 3. Defaulting to 3.");
    }

    if (def.segments > 8)
    {
        def.segments = 8;
        Log().LogError("Polygon segments must be less than or equal to 8. Defaulting to 8.");
    }

    if (!std::isfinite(def.radius) || def.radius <= 0.0f)
    {
        def.radius = 1.0f;
        Log().LogError("Polygon radius must be greater than 0. Defaulting to 1.0f.");
    }

    const BodyId bodyId = rigidBody->GetBodyId();
    const b2BodyId physicsBodyId{ bodyId.index1, bodyId.world0, bodyId.generation };

    if (!b2Body_IsValid(physicsBodyId))
    {
        Log().LogError("Cannot create polygon shape: invalid physics body.");
        return;
    }

    const std::vector<glm::vec2> vertices = MakeRegularPolygonVertices(def.radius, def.segments);

    std::vector<b2Vec2> points;
    points.reserve(vertices.size());

    for (const glm::vec2& vertex : vertices)
    {
        points.push_back({ vertex.x, vertex.y });
    }

    const b2Hull hull = b2ComputeHull(points.data(), static_cast<int>(points.size()));

    if (hull.count < 3)
    {
        Log().LogError("Cannot create polygon shape: hull generation failed.");
        return;
    }

    const b2Polygon polygon = b2MakePolygon(&hull, 0.0f);

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = def.density;
    shapeDef.material.friction = def.friction;
    shapeDef.material.restitution = def.restitution;
    shapeDef.enableContactEvents = true;
    shapeDef.userData = m_Entity;
    shapeDef.isSensor = def.isSensor;

    const b2ShapeId physicsShapeId = b2CreatePolygonShape(physicsBodyId, &shapeDef, &polygon);
    const ShapeId sId = { physicsShapeId.index1, physicsShapeId.world0, physicsShapeId.generation };

    if (!b2Shape_IsValid(physicsShapeId))
    {
        Log().LogError("Cannot create polygon shape: shape creation failed.");
        return;
    }

    SetShapeId(sId);
    Physics().Listen(m_Entity);
}

void buki::Polygon::Set()
{
    if (!m_Entity->GetComponent<RigidBody>())
    {
        return;
    }
    m_Entity->ActivatePhysics();
}