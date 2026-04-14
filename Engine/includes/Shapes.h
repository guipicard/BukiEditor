#pragma once
#include "Component.h"
#include "IDrawable.h"
#include "BukiContainers.h"
#include "stdint.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;	

namespace buki
{
	struct ShapeId
	{
		ShapeId() : index1(0), world0(0), revision(0) {}	
		ShapeId(int32_t i, uint16_t w, uint16_t r) : index1(i), world0(w), revision(r) {}
		int32_t index1;
		uint16_t world0;
		uint16_t revision;
		bool operator==(ShapeId o) { return index1 == o.index1 && world0 == o.world0 && revision == o.revision; }
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

		bool fillDraw = false;
		bool shapeDraw = false;
		Color ShapeColor = Color();
		Color DebugColor = Color();
		bool Sensor = false;
		int filter = 1;
	protected:
		ShapeId shapeId = ShapeId();
	};
}