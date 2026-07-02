#pragma once

#include "Shapes.h"
#include "PropertyInfo.h"

namespace buki
{
    class Circle : public Shapes
    {
    public:
        explicit Circle(Entity* entity) : Shapes(entity) {}
        ~Circle() = default;

        void Draw(float alpha) override;
        void Start() override;
        void Destroy() override;
        void SetPhysics() override;

        void Set() override;

        CircleShapeDef def = DefaultCircleShapeDef();

        const std::vector<PropertyInfo>& GetProperties() const override
        {
            static std::vector<PropertyInfo> properties =
            {
                BUKI_PROP_NESTED_FLOAT_RANGE_GROUPED(BUKI_FIELD_NESTED(Circle, "circle", def, "radius", radius), "Shape", 0.0f, 100.0f, 0.1f),
                BUKI_PROP_NESTED_VECTOR2_GROUPED(BUKI_FIELD_NESTED(Circle, "circle", def, "positionOffset", positionOffset), "Shape"),

                BUKI_PROP_NESTED_BOOL_GROUPED(BUKI_FIELD_NESTED(Circle, "def", def, "fillDraw", fillDraw), "Rendering"),
                BUKI_PROP_NESTED_BOOL_GROUPED(BUKI_FIELD_NESTED(Circle, "def", def, "shapeDraw", shapeDraw), "Rendering"),
                BUKI_PROP_NESTED_COLOR_GROUPED(BUKI_FIELD_NESTED(Circle, "def", def, "fillColor", fillColor), "Rendering"),
                BUKI_PROP_NESTED_COLOR_GROUPED(BUKI_FIELD_NESTED(Circle, "def", def, "shapeColor", shapeColor), "Rendering"),

                BUKI_PROP_NESTED_BOOL_GROUPED(BUKI_FIELD_NESTED(Circle, "def", def, "isSensor", isSensor), "Physics"),
                BUKI_PROP_NESTED_INT_GROUPED(BUKI_FIELD_NESTED(Circle, "def", def, "filter", filter), "Physics"),
                BUKI_PROP_NESTED_FLOAT_RANGE_GROUPED(BUKI_FIELD_NESTED(Circle, "def", def, "density", density), "Physics", 0.0f, 100.0f, 0.1f),
                BUKI_PROP_NESTED_FLOAT_RANGE_GROUPED(BUKI_FIELD_NESTED(Circle, "def", def, "friction", friction), "Physics", 0.0f, 1.0f, 0.01f),
                BUKI_PROP_NESTED_FLOAT_RANGE_GROUPED(BUKI_FIELD_NESTED(Circle, "def", def, "restitution", restitution), "Physics", 0.0f, 1.0f, 0.01f),
            };

            return properties;
        }
    };
}