#pragma once

#include <string>
#include <vector>
#include "imgui.h"

namespace buki
{
	class Component;
	class Entity;
	struct PropertyInfo;
	struct TileDrawData;

	class InspectorPropertyDrawer
	{
	public:
		static bool DrawEntitySection(Entity* entity);
		static bool DrawMultiEntitySection(const std::vector<Entity*>& entities);
		static bool DrawMultiTransformSection(const std::vector<Entity*>& entities);
		// Entry points
		static bool DrawComponent(Component* component, std::string* name);
		static bool DrawProperty(const PropertyInfo& propInfo, void* fieldPtr);

		// Single-property drawers
		static bool DrawIntProperty(const PropertyInfo& propInfo, void* fieldPtr);
		static bool DrawFloatProperty(const PropertyInfo& propInfo, void* fieldPtr);
		static bool DrawBoolProperty(const PropertyInfo& propInfo, void* fieldPtr);
		static bool DrawStringProperty(const PropertyInfo& propInfo, void* fieldPtr);
		static bool DrawStringListProperty(const PropertyInfo& propInfo, void* fieldPtr);
		static bool DrawStringAssetPicker(const PropertyInfo& propInfo, void* fieldPtr, const char* payloadType);
		static bool DrawStringAssetListPicker(const PropertyInfo& propInfo, void* fieldPtr, const char* payloadType);
		static bool DrawEntityPicker(const PropertyInfo& propInfo, void* fieldPtr);
		static bool DrawPrefabPicker(const PropertyInfo& propInfo, void* fieldPtr);
		static bool DrawVector2Property(const PropertyInfo& propInfo, void* fieldPtr);
		static bool DrawColorProperty(const PropertyInfo& propInfo, void* fieldPtr);
		static bool DrawRectFProperty(const PropertyInfo& propInfo, void* fieldPtr);
		static bool DrawEnumIntProperty(const PropertyInfo& propInfo, void* fieldPtr);
		static bool DrawEnumScriptBindingProperty(const PropertyInfo& propInfo, void* fieldPtr);
		static bool DrawTileProperty(const PropertyInfo& propInfo, std::vector<TileDrawData>& tiles, const size_t index, std::vector<size_t>& selectedTiles, bool& openRectWindowRequested, ImVec2& rectWindowAnchor);
		static bool DrawTileListProperty(const PropertyInfo& propInfo, void* fieldPtr);

		// Shared / multi-edit
		static bool DrawSharedComponent(const std::string& componentTypeName, const std::vector<Entity*>& entities);
		static bool DrawCheckboxMixedFallback(const char* label, const char* hiddenId, bool& value, bool mixed);

		// Component add/remove utilities
		static bool DrawAddComponentPopup(Entity* owner);
		static bool DrawAddComponentPopup(const std::vector<Entity*>& owners);
		static std::vector<std::string> GetSharedComponentTypeNames(const std::vector<Entity*>& entities);

	};
}