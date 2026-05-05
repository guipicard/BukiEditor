#pragma once
#include <string>

namespace buki
{
	class Entity;
	class Component;
	struct ShapeDef;
	struct EditorState;

	struct InspectorPanel
	{
		void Render(EditorState& state);

	private:
		bool DrawEntitySection(Entity* entity);
		bool DrawTextComponent(Component* cmp);
		bool DrawButtonComponent(Component* cmp);
		bool DrawSpriteComponent(Component* cmp);
		bool DrawTileLayerComponent(Component* cmp, EditorState& state);
		bool DrawRigidBodyComponent(Component* cmp);
		bool DrawShapeCommonFields(ShapeDef& def);
		bool DrawBoxComponent(Component* cmp);
		bool DrawCircleComponent(Component* cmp);
		bool DrawPolygonComponent(Component* cmp);
		std::string RemoveComponent(Component* cmp);
	};
}