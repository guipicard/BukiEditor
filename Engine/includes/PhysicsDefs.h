#pragma once

#include "BukiContainers.h"

namespace buki
{
    struct MotionLocks
    {
        bool linearX = false;
        bool linearY = false;
        bool angularZ = false;
    };

    struct RigidBodyDef
    {
        enum class BodyType
        {
            Static = 0,
            Kinematic = 1,
            Dynamic = 2,
        };

        BodyType type = BodyType::Dynamic;
        MotionLocks motionLocks{};
    };

    inline RigidBodyDef DefaultRigidBodyDef()
    {
		RigidBodyDef def{};
		def.type = RigidBodyDef::BodyType::Dynamic;
		def.motionLocks.linearX = false;
		def.motionLocks.linearY = false;
		def.motionLocks.angularZ = false;
        return def;
    }

    struct ShapeDef
    {
        bool fillDraw;
        bool shapeDraw;
        Color shapeColor;
        Color fillColor;
        bool isSensor;
        int filter;
        float density;
        float friction;
        float restitution;
		//float angleOffset;
		Vector2 positionOffset;
    };

    inline ShapeDef DefaultShapeDef()
    {
		ShapeDef def{};
		def.fillDraw = false;
        def.shapeDraw = false;
		def.shapeColor = { 0.0f, 1.0f, 0.0f, 1.0f };
		def.fillColor = { 1.0f, 0.0f, 0.0f, 0.3f };
		def.isSensor = false;
		def.filter = 0;
		def.density = 1.0f;
		def.friction = 0.6f;
		def.restitution = 0.0f;
		//def.angleOffset = 0.0f;
		def.positionOffset = { 0.0f, 0.0f };
        return def;
    }

    struct BoxShapeDef : public ShapeDef
    {
        Vector2 size;
    };

    inline BoxShapeDef DefaultBoxShapeDef()
    {
        BoxShapeDef def{};
        def.fillDraw = false;
        def.shapeDraw = false;
        def.fillColor = { 0.0f, 1.0f, 1.0f, 0.3f };
        def.shapeColor = { 1.0f, 0.0f, 0.0f, 1.0f };
        def.isSensor = false;
        def.filter = 0;
        def.density = 1.0f;
        def.friction = 0.6f;
        def.restitution = 0.0f;
        //def.angleOffset = 0.0f;
        def.positionOffset = { 0.0f, 0.0f };

		def.size = { 1.0f, 1.0f };
		def.positionOffset = { 0.0f, 0.0f };
        return def;
    }

    struct CircleShapeDef : public ShapeDef
    {
        float radius;
    };

    inline CircleShapeDef DefaultCircleShapeDef()
    {
        CircleShapeDef def{};
        def.fillDraw = false;
        def.shapeDraw = false;
        def.fillColor = { 1.0f, 0.0f, 0.0f, 0.3f };
        def.shapeColor = { 0.0f, 1.0f, 0.0f, 1.0f };
        def.isSensor = false;
        def.filter = 0;
        def.density = 1.0f;
        def.friction = 0.6f;
        def.restitution = 0.0f;
        //def.angleOffset = 0.0f;
        def.positionOffset = { 0.0f, 0.0f };

        def.radius = 1.0f;
        return def;
    }

    struct PolygonShapeDef : public ShapeDef
    {
		float radius;
		int segments;
    };

    inline PolygonShapeDef DefaultPolygonShapeDef()
    {
        PolygonShapeDef def{};
        def.fillDraw = false;
        def.shapeDraw = false;
        def.fillColor = { 1.0f, 1.0f, 0.0f, 0.3f };
        def.shapeColor = { 0.0f, 0.0f, 1.0f, 1.0f };
        def.isSensor = false;
        def.filter = 0;
        def.density = 1.0f;
        def.friction = 0.6f;
        def.restitution = 0.0f;
        //def.angleOffset = 0.0f;
        def.positionOffset = { 0.0f, 0.0f };

        def.radius = 1.0f;
        def.segments = 3;
        return def;
    }
}
