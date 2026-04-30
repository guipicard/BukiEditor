#pragma once
#include "MonoBehaviour.h"

namespace buki
{
	struct Spawner;
	struct Sprite;
	struct Collider;
	struct RigidBody;
	struct Text;
	struct Button;

	struct ShapesController : public MonoBehaviour
	{
		ShapesController(Entity* entity);
		~ShapesController() = default;
		virtual void Awake() override;
		virtual void Start() override;
		virtual void FixedUpdate(const float dt) override;
		virtual void Update(const float dt) override;
		virtual void OnCollisionEnter(Entity* other) override;
		virtual void OnCollisionExit(Entity* other) override;
		virtual void OnCollisionHit(Entity* other) override;
		virtual void OnSensorEnter(Entity* other) override;
		virtual void OnSensorExit(Entity* other) override;

		virtual json Serialize() override;
		virtual void Deserialize(json _doc) override;
		virtual void Set() override;

		inline void DrawCircleFill(const bool state) { circleFillDraw = state; }
		inline void DrawCircleShape(const bool state) { circleShapeDraw = state; }

		inline void DrawBoxFill(const bool state) { boxFillDraw = state; }
		inline void DrawBoxShape(const bool state) { boxShapeDraw = state; }
	private:
		Vector2 firstMousePos = { 0.0f,0.0f };

		Spawner* spawner;
		
		bool circleFillDraw = true;
		bool circleShapeDraw = true;

		bool boxFillDraw = true;
		bool boxShapeDraw = true;

		std::vector<Button*> buttonsInScene;
		bool UIHovered = false;
	};
}
