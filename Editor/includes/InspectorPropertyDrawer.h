#pragma once

#include <string>
#include <vector>

namespace buki
{
	class Component;
	class MonoBehaviour;
	class Entity;
	struct PropertyInfo;

	class InspectorPropertyDrawer
	{
	public:
		// Entry points
		static bool DrawComponent(Component* component, std::string* name);
		static bool DrawMonoBehaviour(MonoBehaviour* behaviour, std::string* name);
		static bool DrawProperty(const PropertyInfo& prop, void* fieldPtr);

		// Single-property drawers
		static bool DrawIntProperty(const PropertyInfo& prop, void* fieldPtr);
		static bool DrawFloatProperty(const PropertyInfo& prop, void* fieldPtr);
		static bool DrawBoolProperty(const PropertyInfo& prop, void* fieldPtr);
		static bool DrawStringProperty(const PropertyInfo& prop, void* fieldPtr);
		static bool DrawStringListProperty(const PropertyInfo& prop, void* fieldPtr);
		static bool DrawStringAssetPicker(const PropertyInfo& prop, void* fieldPtr, const char* payloadType);
		static bool DrawStringAssetListPicker(const PropertyInfo& prop, void* fieldPtr, const char* payloadType);
		static bool DrawSpriteTexturePathField(const char* label, std::string& texturePath);
		static bool DrawEntityPicker(const PropertyInfo& prop, void* fieldPtr);
		static bool DrawPrefabPicker(const PropertyInfo& prop, void* fieldPtr);

		// Shared / multi-edit
		static bool DrawSharedComponent(const std::string& componentTypeName, const std::vector<Entity*>& entities);
		static bool DrawCheckboxMixedFallback(const char* label, const char* hiddenId, bool& value, bool mixed);

		// Component add/remove utilities
		static bool DrawAddComponentPopup(Entity* owner);
		static bool DrawAddComponentPopup(const std::vector<Entity*>& owners);
		static std::vector<std::string> GetSharedComponentTypeNames(const std::vector<Entity*>& entities);

		static void ClearCachedEditorState();
	};
}