#pragma once

#include "Component.h"
#include "IFixedUpdatable.h"
#include "PhysicsDefs.h"
#include "stdint.h"

namespace buki
{
    struct BodyId
    {
        BodyId() : index1(0), world0(0), generation(0) {}
        BodyId(int32_t i, uint16_t w, uint16_t r) : index1(i), world0(w), generation(r) {}
        int32_t index1;
        uint16_t world0;
        uint16_t generation;
    };

    struct RigidBody : public Component, public IFixedUpdatable
    {
        using BodyType = RigidBodyDef::BodyType;

        RigidBody(Entity* entity) { m_Entity = entity; }
        ~RigidBody() = default;

        void FixedUpdate(float dt) override;
        void Start() override;
        void Destroy() override;

        json Serialize() override;
        void Deserialize(json _doc) override;
        void Set() override;

        BodyId GetBodyId() const { return bodyId; }
        void SetBodyId(BodyId _id) { bodyId = _id; }

        float GetMass() const;
        Vector2 GetVelocity() const;
        void SetForce(Vector2 _force);

        RigidBodyDef def = DefaultRigidBodyDef();

    private:
        BodyId bodyId = BodyId();
    };
}
