#pragma once

#include "Shapes.h"
#include "PhysicsDefs.h"
#include "Graphics/Texture2D.h"
#include "PropertyInfo.h"

namespace buki
{
    class Box : public Shapes
    {
    public:
        explicit Box(Entity* entity) : Shapes(entity) {}
        ~Box() = default;

        void Draw(float alpha) override;
        void Start() override;
        void Destroy() override;
        void SetPhysics() override;

        void Set() override;

        BoxShapeDef def = DefaultBoxShapeDef();

        const std::vector<PropertyInfo>& GetProperties() const override
        {
            static std::vector<PropertyInfo> properties =
            {
                BUKI_PROP_NESTED_VECTOR2_GROUPED(BUKI_FIELD_NESTED(Box, "box", def, "size", size), "Shape"),
                BUKI_PROP_NESTED_VECTOR2_GROUPED(BUKI_FIELD_NESTED(Box, "def", def, "positionOffset", positionOffset), "Shape"),

                BUKI_PROP_NESTED_BOOL_GROUPED(BUKI_FIELD_NESTED(Box, "def", def, "fillDraw", fillDraw), "Rendering"),
                BUKI_PROP_NESTED_BOOL_GROUPED(BUKI_FIELD_NESTED(Box, "def", def, "shapeDraw", shapeDraw), "Rendering"),
                BUKI_PROP_NESTED_COLOR_GROUPED(BUKI_FIELD_NESTED(Box, "def", def, "fillColor", fillColor), "Rendering"),
                BUKI_PROP_NESTED_COLOR_GROUPED(BUKI_FIELD_NESTED(Box, "def", def, "shapeColor", shapeColor), "Rendering"),

                BUKI_PROP_NESTED_BOOL_GROUPED(BUKI_FIELD_NESTED(Box, "def", def, "isSensor", isSensor), "Physics"),
                BUKI_PROP_NESTED_INT_GROUPED(BUKI_FIELD_NESTED(Box, "def", def, "filter", filter), "Physics"),
                BUKI_PROP_NESTED_FLOAT_RANGE_GROUPED(BUKI_FIELD_NESTED(Box, "def", def, "density", density), "Physics", 0.0f, 100.0f, 0.1f),
                BUKI_PROP_NESTED_FLOAT_RANGE_GROUPED(BUKI_FIELD_NESTED(Box, "def", def, "friction", friction), "Physics", 0.0f, 1.0f, 0.01f),
                BUKI_PROP_NESTED_FLOAT_RANGE_GROUPED(BUKI_FIELD_NESTED(Box, "def", def, "restitution", restitution), "Physics", 0.0f, 1.0f, 0.01f),
            };

            return properties;
        }
    };
}