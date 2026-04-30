#pragma once

#include "Shapes.h"
#include "BukiContainers.h"
#include "PhysicsDefs.h"

namespace buki
{
    struct Polygon : public Shapes
    {
        Polygon(Entity* entity) : Shapes(entity) {}
        ~Polygon() = default;

        virtual void Draw(float alpha) override;
        virtual void Start() override;
        virtual void Destroy() override;
        virtual void SetPhysics() override;

        virtual json Serialize() override;
        virtual void Deserialize(json _doc) override;
        virtual void Set() override;

        PolygonShapeDef def = DefaultPolygonShapeDef();
    };
}