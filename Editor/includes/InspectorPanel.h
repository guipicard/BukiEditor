#pragma once
#include <string>
#include <vector>
#include <filesystem>

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
	struct PrefabPreviewSession;
	class IWorld;

	struct InspectorPanel
	{
		void Render(EditorState& state);

	private:

		void RenderSingleEntityInspector(Entity* entity, EditorState& state, IWorld& world);
		void RenderMultiEntityInspector(const std::vector<Entity*>& selected, EditorState& state, IWorld& world);

		std::string RemoveComponent(Component* cmp);
		bool SaveSelectedEntityAsPrefab(Entity* entity);

		void DrawPrefabPreviewWindows(EditorState& state);
		bool DrawPrefabEntitySection(Entity* entity);
		void RenderPrefabInspector(EditorState& state, IWorld& world);
		//void DestroyPrefabPreviewFramebuffer(PrefabPreviewSession& session);
	};
}