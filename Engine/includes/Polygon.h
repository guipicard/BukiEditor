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
        void Draw(float alpha) override;
        void Start() override;
        void Destroy() override;
        void SetPhysics() override;
        json Serialize() override;
        void Deserialize(json _doc) override;
        void Set() override;

    public:
        PolygonShapeDef def = DefaultPolygonShapeDef();
    };
}