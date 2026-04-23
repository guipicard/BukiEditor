#pragma once
#include "Component.h"
#include "IDrawable.h"
#include "PhysicsDefs.h"
#include "stdint.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;	

namespace buki
{
	struct ShapeId
	{
		int32_t index1;
		uint16_t world0;
		uint16_t generation;
		//bool operator==(ShapeId o) { return index1 == o.index1 && world0 == o.world0 && generation == o.generation; }
	};

	struct Shapes : public Component, public IDrawable
	{
		Shapes(Entity* entity) { m_Entity = entity; }
		virtual ~Shapes() = default;

		virtual void SetPhysics() = 0;
		virtual json Serialize() override;
		virtual void Deserialize(json _doc) override;


		void SetShapeId(ShapeId id) { shapeId = id; }
		ShapeId GetShapeId() const { return shapeId; }
	protected:
		ShapeId shapeId = ShapeId();
	};
}