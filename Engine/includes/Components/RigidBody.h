#pragma once

#include "Component.h"
#include "IFixedUpdatable.h"
#include "PhysicsDefs.h"
#include "stdint.h"
#include "PropertyInfo.h"

namespace buki
{
    struct BodyId
    {
        BodyId() : index1(-1), world0(-1), generation(-1) {}
        BodyId(int32_t i, uint16_t w, uint16_t r) : index1(i), world0(w), generation(r) {}

        int32_t index1;
        uint16_t world0;
        uint16_t generation;
    };

    class RigidBody : public Component, public IFixedUpdatable
    {
    public:
        using BodyType = RigidBodyDef::BodyType;

        explicit RigidBody(Entity* entity) { m_Entity = entity; }
        ~RigidBody() = default;

        void FixedUpdate(float dt) override;
        void Start() override;
        void Destroy() override;

        void Set() override;

        BodyId GetBodyId() const { return bodyId; }
        void SetBodyId(BodyId _id) { bodyId = _id; }

        float GetMass() const;
        Vector2 GetVelocity() const;
        void SetForce(Vector2 _force);

        RigidBodyDef def = DefaultRigidBodyDef();

        const std::vector<PropertyInfo>& GetProperties() const override
        {
            static std::vector<PropertyInfo> properties =
            {
                BUKI_PROP_ENUM_INT_GROUPED("type", RigidBody, def.type, "Body", "Static", "Kinematic", "Dynamic"),

                BUKI_PROP_NESTED_BOOL_GROUPED(BUKI_FIELD_NESTED2(RigidBody, "def", def, "motionLocks", motionLocks, "linearX", linearX), "Motion Locks"),
                BUKI_PROP_NESTED_BOOL_GROUPED(BUKI_FIELD_NESTED2(RigidBody, "def", def, "motionLocks", motionLocks, "linearY", linearY), "Motion Locks"),
                BUKI_PROP_NESTED_BOOL_GROUPED(BUKI_FIELD_NESTED2(RigidBody, "def", def, "motionLocks", motionLocks, "angularZ", angularZ), "Motion Locks"),
            };

            return properties;
        }

    private:
        BodyId bodyId = BodyId();
    };
}