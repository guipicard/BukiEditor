#pragma once

#include "Shapes.h"
#include "BukiContainers.h"
#include "PhysicsDefs.h"
#include "PropertyInfo.h"

namespace buki
{
    class Polygon : public Shapes
    {
    public:
        explicit Polygon(Entity* entity) : Shapes(entity) {}
        ~Polygon() = default;

        void Draw(float alpha) override;
        void Start() override;
        void Destroy() override;
        void SetPhysics() override;

        void Set() override;

        PolygonShapeDef def = DefaultPolygonShapeDef();

        const std::vector<PropertyInfo>& GetProperties() const override
        {
            static std::vector<PropertyInfo> properties =
            {
                BUKI_PROP_NESTED_FLOAT_RANGE_GROUPED(BUKI_FIELD_NESTED(Polygon, "polygon", def, "radius", radius), "Shape", 0.0f, 100.0f, 0.1f),
                BUKI_PROP_NESTED_INT_RANGE_GROUPED(BUKI_FIELD_NESTED(Polygon, "polygon", def, "segments", segments), "Shape", 3, 64, 1),
                BUKI_PROP_NESTED_VECTOR2_GROUPED(BUKI_FIELD_NESTED(Polygon, "polygon", def, "positionOffset", positionOffset), "Shape"),

                BUKI_PROP_NESTED_BOOL_GROUPED(BUKI_FIELD_NESTED(Polygon, "polygon", def, "fillDraw", fillDraw), "Rendering"),
                BUKI_PROP_NESTED_BOOL_GROUPED(BUKI_FIELD_NESTED(Polygon, "polygon", def, "shapeDraw", shapeDraw), "Rendering"),
                BUKI_PROP_NESTED_COLOR_GROUPED(BUKI_FIELD_NESTED(Polygon, "polygon", def, "fillColor", fillColor), "Rendering"),
                BUKI_PROP_NESTED_COLOR_GROUPED(BUKI_FIELD_NESTED(Polygon, "polygon", def, "shapeColor", shapeColor), "Rendering"),

                BUKI_PROP_NESTED_BOOL_GROUPED(BUKI_FIELD_NESTED(Polygon, "polygon", def, "isSensor", isSensor), "Physics"),
                BUKI_PROP_NESTED_INT_GROUPED(BUKI_FIELD_NESTED(Polygon, "polygon", def, "filter", filter), "Physics"),
                BUKI_PROP_NESTED_FLOAT_RANGE_GROUPED(BUKI_FIELD_NESTED(Polygon, "polygon", def, "density", density), "Physics", 0.0f, 100.0f, 0.1f),
                BUKI_PROP_NESTED_FLOAT_RANGE_GROUPED(BUKI_FIELD_NESTED(Polygon, "polygon", def, "friction", friction), "Physics", 0.0f, 1.0f, 0.01f),
                BUKI_PROP_NESTED_FLOAT_RANGE_GROUPED(BUKI_FIELD_NESTED(Polygon, "polygon", def, "restitution", restitution), "Physics", 0.0f, 1.0f, 0.01f),
            };

            return properties;
        }
    };
}