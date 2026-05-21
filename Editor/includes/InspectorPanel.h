#pragma once
#include <string>
#include <vector>

namespace buki
{
	class Entity;
	class Component;
	class Text;
	class Button;
	class Sprite;
	class RigidBody;
	class Box;
	class Circle;
	class Polygon;
	struct ShapeDef;
	struct EditorState;

	struct InspectorPanel
	{
		void Render(EditorState& state);

	private:
		bool DrawEntitySection(Entity* entity);
		bool DrawMultiEntitySection(const std::vector<Entity*>& entities);
		bool DrawMultiTransformSection(const std::vector<Entity*>& entities);

		bool DrawTextComponent(Component* cmp);
		bool DrawButtonComponent(Component* cmp);
		bool DrawSpriteComponent(Component* cmp);
		bool DrawTileLayerComponent(Component* cmp, EditorState& state);
		bool DrawRigidBodyComponent(Component* cmp);
		bool DrawShapeCommonFields(ShapeDef& def);
		bool DrawBoxComponent(Component* cmp);
		bool DrawCircleComponent(Component* cmp);
		bool DrawPolygonComponent(Component* cmp);

		bool DrawSharedTextComponents(const std::vector<Entity*>& entities);
		bool DrawSharedButtonComponents(const std::vector<Entity*>& entities);
		bool DrawSharedSpriteComponents(const std::vector<Entity*>& entities);
		bool DrawSharedRigidBodyComponents(const std::vector<Entity*>& entities);
		bool DrawSharedBoxComponents(const std::vector<Entity*>& entities);
		bool DrawSharedCircleComponents(const std::vector<Entity*>& entities);
		bool DrawSharedPolygonComponents(const std::vector<Entity*>& entities);
		bool DrawSharedShapeCommonFields(const std::vector<ShapeDef*>& defs);

		std::string RemoveComponent(Component* cmp);
		bool SaveSelectedEntityAsPrefab(Entity* entity);

		bool DrawImageAssetPathPicker(const char* label, std::string& path);
		bool DrawSharedImageAssetPathPicker(const char* label, std::vector<std::string*>& paths);
		
		bool DrawMixedStringField(const char* label, bool mixed, std::string& value);
		bool DrawMixedIntField(const char* label, bool mixed, int& value);
		bool DrawMixedFloatField(const char* label, bool mixed, float& value, const char* format = "%.3f");
		bool DrawMixedBoolField(const char* label, bool mixed, bool& value);
		bool DrawMixedFloat2Field(const char* label, bool mixed, float values[2], const char* format = "%.3f");
		bool DrawMixedFloat4Field(const char* label, bool mixed, float values[4], const char* format = "%.3f");
		bool DrawMixedColorField(const char* label, bool mixed, float values[4]);
		bool DrawMixedComboField(const char* label, bool mixed, int& currentIndex, const char* const items[], int itemsCount);

		bool DrawMixedVector2FieldPerAxis(
			const char* label,
			bool mixedX,
			bool mixedY,
			float& x,
			float& y,
			bool& changedX,
			bool& changedY,
			const char* format = "%.3f");
		bool DrawMixedVector2DragDeltaFieldPerAxis(
			const char* label,
			bool mixedX,
			bool mixedY,
			float& x,
			float& y,
			float& deltaX,
			float& deltaY,
			bool& changedX,
			bool& changedY,
			const char* format = "%.3f");
	};
}