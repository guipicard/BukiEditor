#pragma once
#include "Component.h"
#include "IUpdatable.h"
#include "IFixedUpdatable.h"
#include "Vector2.h"
#include "Material.h"
#include "MassData.h"
#include "Entity.h"
#include "stdint.h"

namespace buki
{
	

	struct BodyId
	{
		BodyId() : index1(0), world0(0), revision(0) {}
		BodyId(int32_t i, uint16_t w, uint16_t r) : index1(i), world0(w), revision(r) {}
		int32_t index1;
		uint16_t world0;
		uint16_t revision;
	};

	struct RigidBody : public Component, public IFixedUpdatable
	{
		enum class BodyType {
			Static = 0,

			/// zero mass, velocity set by user, moved by solver
			kinematic = 1,

			/// positive mass, velocity determined by forces, moved by solver
			dynamic = 2,

			/// number of body types
			bodyTypeCount,
		};
		BodyType Type = BodyType::dynamic;
		bool FixedRotation = false;

		RigidBody(Entity* entity) { m_Entity = entity; }
		~RigidBody() = default;
		virtual void FixedUpdate(float dt) override;
		virtual void Start() override;
		virtual void Destroy() override;

		virtual json Serialize() override;
		virtual void Deserialize(json _doc) override;
		virtual void Set() override;

		BodyId GetBodyId() const { return bodyId; }
		void SetBodyId(BodyId _id) { bodyId = _id; }

		float GetMass() const;
		Vector2 GetVelocity() const;
		void SetForce(Vector2 _force);
	private:
		BodyId bodyId = BodyId();
	};
}
