#pragma once
#include "MonoBehaviour.h"
#include "EntityRef.h"
#include "ScriptFunctionRegistry.h"
#include "IWorld.h"

namespace buki
{
	class Sprite;
	class RigidBody;
	class Text;
	class Button;

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

		virtual void OnSet() override;

	public:
		const std::vector<PropertyInfo>& GetProperties() const override
		{
			static std::vector<PropertyInfo> properties = {
				BUKI_PROP_BOOL(ShapesController, circleFillDraw),
				BUKI_PROP_BOOL(ShapesController, circleShapeDraw),
				BUKI_PROP_BOOL(ShapesController, boxFillDraw),
				BUKI_PROP_BOOL(ShapesController, boxShapeDraw),
				BUKI_PROP_BOOL(ShapesController, polygonFillDraw),
				BUKI_PROP_BOOL(ShapesController, polygonShapeDraw),
				BUKI_PROP_PREFAB(ShapesController, boxRef),
				BUKI_PROP_PREFAB(ShapesController, circleRef),
				BUKI_PROP_PREFAB(ShapesController, polygonRef),
				BUKI_PROP_INT(ShapesController, polygonSides),

			};
			return properties;
		}
		bool circleFillDraw = true;
		bool circleShapeDraw = true;
		bool boxFillDraw = true;
		bool boxShapeDraw = true;
		bool polygonFillDraw = true;
		bool polygonShapeDraw = true;
		int polygonSides = 3;

		PrefabAssetInstance boxRef;
		PrefabAssetInstance circleRef;
		PrefabAssetInstance polygonRef;
	public:
		inline void SetDrawCircleFill(const bool state) { circleFillDraw = state; }
		inline void SetDrawCircleShape(const bool state) { circleShapeDraw = state; }

		inline void SetDrawBoxFill(const bool state) { boxFillDraw = state; }
		inline void SetDrawBoxShape(const bool state) { boxShapeDraw = state; }

		inline void SetDrawPolygonFill(const bool state) { polygonFillDraw = state; }
		inline void SetDrawPolygonShape(const bool state) { polygonShapeDraw = state; }
	private:
		Vector2 firstMousePos = { 0.0f,0.0f };

		std::vector<Button*> buttonsInScene;
		bool UIHovered = false;
	
	public:
		static void DrawBoxFill();
		static void DrawBoxShape();
		static void DrawBoxBoth();

		static void DrawCircleFill();
		static void DrawCircleShape();
		static void DrawCircleBoth();

		static void DrawPolygonFill();
		static void DrawPolygonShape();
		static void DrawPolygonBoth();
	};
}


