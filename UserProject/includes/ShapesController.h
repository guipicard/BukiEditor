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

		virtual void Start() override;
		virtual void Destroy() override;

		virtual void FixedUpdate(const float dt) override;
		virtual void Update(const float dt) override;

		virtual void OnCollisionEnter(Entity* other) override;
		virtual void OnCollisionExit(Entity* other) override;
		virtual void OnCollisionHit(Entity* other) override;
		virtual void OnSensorEnter(Entity* other) override;
		virtual void OnSensorExit(Entity* other) override;

		virtual void Set() override;

	public:
		const std::vector<PropertyInfo>& GetProperties() const override
		{
			static std::vector<PropertyInfo> properties = {
				BUKI_PROP_BOOL(ShapesController, circleFillDraw),
				BUKI_PROP_BOOL(ShapesController, circleShapeDraw),
				BUKI_PROP_BOOL(ShapesController, boxFillDraw),
				BUKI_PROP_BOOL(ShapesController, boxShapeDraw)
			};
			return properties;
		}
		bool circleFillDraw = true;
		bool circleShapeDraw = true;
		bool boxFillDraw = true;
		bool boxShapeDraw = true;

	public:
		inline void DrawCircleFill(const bool state) { circleFillDraw = state; }
		inline void DrawCircleShape(const bool state) { circleShapeDraw = state; }

		inline void DrawBoxFill(const bool state) { boxFillDraw = state; }
		inline void DrawBoxShape(const bool state) { boxShapeDraw = state; }
	private:
		Vector2 firstMousePos = { 0.0f,0.0f };

		Spawner* spawner;


		std::vector<Button*> buttonsInScene;
		bool UIHovered = false;
	};
}
