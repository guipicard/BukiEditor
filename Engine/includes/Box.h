#pragma once

#include "Shapes.h"
#include "BoxCollider.h"
#include "PhysicsDefs.h"
#include "Graphics/Texture2D.h"

namespace buki
{
    struct Box : public Shapes
    {
        Box(Entity* entity) : Shapes(entity) {}
        ~Box() = default;

        void Draw(float alpha) override;
        void Start() override;
        void Destroy() override;
        void SetPhysics() override;

        json Serialize() override;
        void Deserialize(json _doc) override;
        void Set() override;

        BoxShapeDef def = DefaultBoxShapeDef();
    };
}
