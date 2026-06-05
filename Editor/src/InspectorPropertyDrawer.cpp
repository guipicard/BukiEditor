#pragma once
#include "InspectorPropertyDrawer.h"

#include "Component.h"
#include "ComponentFactory.h"
#include "Engine.h"
#include "Entity.h"
#include "EntityRef.h"
#include "IWorld.h"
#include "MonoBehaviour.h"
#include "PropertyInfo.h"
#include "imgui.h"
#include "EditorAssetEntries.h"

namespace
{
	struct MixedTextState
	{
		bool initialized = false;
		std::string value;
	};

	struct ListEditState
	{
		bool initialized = false;
		std::vector<std::string> values;
	};

	static std::unordered_map<ImGuiID, MixedTextState> sMixedTextStates;
	static std::unordered_map<ImGuiID, ListEditState> sListEditStates;
	static char sAssetSearchBuffer[256] = {};

	template<typename T>
	bool AreAllEqualValues(const std::vector<T>& values)
	{
		if (values.empty())
			return true;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (!(values[i] == values[0]))
				return false;
		}
		return true;
	}

	bool AreAllEqualEntityRefs(const std::vector<buki::EntityRef*>& refs)
	{
		if (refs.empty())
			return true;

		const buki::EntityRef* first = refs[0];
		if (first == nullptr)
			return false;

		for (size_t i = 1; i < refs.size(); ++i)
		{
			const buki::EntityRef* current = refs[i];
			if (current == nullptr)
				return false;

			if (current->entityName != first->entityName)
				return false;
			if (current->prefabPath != first->prefabPath)
				return false;
		}

		return true;
	}

	template<typename T>
	std::vector<T*> GatherSharedFieldPtrs(
		const std::string& componentTypeName,
		const std::vector<buki::Entity*>& entities,
		size_t offset)
	{
		std::vector<T*> result;
		result.reserve(entities.size());

		for (buki::Entity* entity : entities)
		{
			if (entity == nullptr)
				return {};

			buki::Component* component = entity->GetComponentByTypeName(componentTypeName);
			if (component == nullptr)
				return {};

			char* base = reinterpret_cast<char*>(component);
			result.push_back(reinterpret_cast<T*>(base + offset));
		}

		return result;
	}

	template<typename T>
	void ApplySharedValue(const std::vector<T*>& values, const T& value)
	{
		for (T* ptr : values)
		{
			if (ptr != nullptr)
				*ptr = value;
		}
	}

	MixedTextState& GetMixedTextState(ImGuiID id, const std::string& initialValue)
	{
		MixedTextState& state = sMixedTextStates[id];
		if (!state.initialized)
		{
			state.initialized = true;
			state.value = initialValue;
		}
		return state;
	}

	void ResetMixedTextState(ImGuiID id)
	{
		sMixedTextStates.erase(id);
	}

	ListEditState& GetListEditState(ImGuiID id, const std::vector<std::string>& initialValues)
	{
		ListEditState& state = sListEditStates[id];
		if (!state.initialized)
		{
			state.initialized = true;
			state.values = initialValues;
		}
		return state;
	}

	void ResetListEditState(ImGuiID id)
	{
		sListEditStates.erase(id);
	}

	void DrawMixedLabel(const std::string& label)
	{
		ImGui::TextUnformatted(label.c_str());
		ImGui::SameLine();
		ImGui::TextDisabled("(mixed)");
	}

	bool DrawMixedStringEditor(const char* label, bool mixed, std::string& value, float width = 220.0f)
	{
		bool changed = false;

		ImGui::PushID(label);
		const ImGuiID fieldId = ImGui::GetID("MixedStringField");

		const std::string initialDisplay = mixed ? "-" : value;
		MixedTextState& state = GetMixedTextState(fieldId, initialDisplay);

		char buffer[512] = {};
		strncpy_s(buffer, state.value.c_str(), sizeof(buffer) - 1);

		ImGui::TextUnformatted(label);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(width);

		if (ImGui::InputText("##MixedStringField", buffer, sizeof(buffer)))
		{
			state.value = buffer;
			if (state.value != "-")
			{
				value = state.value;
				changed = true;
			}
		}

		if (!ImGui::IsItemActive())
			ResetMixedTextState(fieldId);

		ImGui::PopID();
		return changed;
	}

	bool DrawAssetPickerPopupGrid(const char* popupId, const char* payloadType, std::string& path)
	{
		bool changed = false;
		if (!ImGui::BeginPopupModal(popupId, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			return false;

		ImGui::SetNextItemWidth(320.0f);
		ImGui::InputTextWithHint("##AssetSearch", "Search assets...", sAssetSearchBuffer, sizeof(sAssetSearchBuffer));

		ImGui::SameLine();
		if (ImGui::Button("Clear Search"))
			sAssetSearchBuffer[0] = '\0';

		ImGui::Separator();

		if (ImGui::Button("None", ImVec2(120.0f, 0.0f)))
		{
			path.clear();
			changed = true;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Close", ImVec2(120.0f, 0.0f)))
		{
			ImGui::CloseCurrentPopup();
		}

		static std::unordered_map<std::string, std::vector<buki::BrowserEntry>> sCachedEntries;

		const std::string cacheKey = std::string(payloadType);
		if (sCachedEntries.find(cacheKey) == sCachedEntries.end())
			sCachedEntries[cacheKey] = buki::CollectBrowserEntries("../Deployment", payloadType);

		ImGui::SameLine();
		if (ImGui::Button("Refresh", ImVec2(120.0f, 0.0f)))
		{
			sCachedEntries[cacheKey] = buki::CollectBrowserEntries("../Deployment", payloadType);
		}

		const std::vector<buki::BrowserEntry>& allEntries = sCachedEntries[cacheKey];
		const std::string filter = buki::ToLowerCopy(sAssetSearchBuffer);

		const float thumbnailSize = 80.0f;
		const float padding = thumbnailSize/4.0f;
		const float pickerWidth = 720.0f;
		const float pickerHeight = 430.0f;
		const float cellSize = thumbnailSize + padding;

		int columnCount = static_cast<int>(pickerWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::BeginChild("##AssetPickerGrid", ImVec2(pickerWidth, pickerHeight), true);
		ImGui::Columns(columnCount, nullptr, false);

		for (const buki::BrowserEntry& item : allEntries)
		{
			const std::string full = buki::ToLowerCopy(item.fullPath.string());
			const std::string file = buki::ToLowerCopy(item.displayName);

			if (!filter.empty() &&
				full.find(filter) == std::string::npos &&
				file.find(filter) == std::string::npos)
			{
				continue;
			}

			const bool selected = (!path.empty() && fs::path(path).lexically_normal() == item.fullPath);
			const bool clicked = buki::DrawBrowserTile(item, thumbnailSize, selected);
			const bool doubleClicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);

			if (clicked)
			{
				path = item.fullPath.string();
				changed = true;

			}

			if (doubleClicked)
			{
				path = item.fullPath.string();
				changed = true;
				ImGui::CloseCurrentPopup();
			}

			ImGui::NextColumn();
		}

		ImGui::Columns(1);
		ImGui::EndChild();

		ImGui::EndPopup();
		return changed;
	}

	bool DrawAssetListEditor(
		const char* label,
		std::vector<std::string>& values,
		const char* payloadType,
		bool mixed,
		bool& applyRequested,
		bool& clearRequested)
	{
		bool changed = false;
		applyRequested = false;
		clearRequested = false;

		ImGui::Text("%s", label);
		ImGui::Indent();

		if (mixed)
		{
			ImGui::TextDisabled("(mixed)");
			ImGui::TextDisabled("Editing this list will replace the values on all selected entities.");
		}

		int removeIndex = -1;
		for (int i = 0; i < static_cast<int>(values.size()); ++i)
		{
			ImGui::PushID(i);

			std::string displayName = buki::DisplayNameForPath(values[i]);
			if (displayName.empty())
				displayName = "None";

			if (ImGui::Button(displayName.c_str(), ImVec2(260.0f, 0.0f)))
				ImGui::OpenPopup("AssetItemPicker");

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadType))
				{
					const char* dropped = static_cast<const char*>(payload->Data);
					if (dropped != nullptr)
					{
						values[i] = dropped;
						changed = true;
					}
				}
				ImGui::EndDragDropTarget();
			}

			changed |= DrawAssetPickerPopupGrid("AssetItemPicker", payloadType, values[i]);

			ImGui::SameLine();
			if (ImGui::Button("Clear"))
			{
				values[i].clear();
				changed = true;
			}

			ImGui::SameLine();
			if (ImGui::Button("-"))
				removeIndex = i;

			ImGui::PopID();
		}

		if (removeIndex >= 0)
		{
			values.erase(values.begin() + removeIndex);
			changed = true;
		}

		if (ImGui::Button(("Add##" + std::string(label)).c_str()))
		{
			values.push_back("");
			changed = true;
		}

		if (mixed)
		{
			if (changed)
				applyRequested = true;

			if (changed)
				clearRequested = true;
		}

		ImGui::Unindent();
		return changed;
	}

	bool DrawSharedIntProperty(const buki::PropertyInfo& prop, const std::vector<int*>& values)
	{
		if (values.empty())
			return false;

		std::vector<int> snapshot;
		snapshot.reserve(values.size());
		for (int* ptr : values)
			snapshot.push_back(*ptr);

		const bool mixed = !AreAllEqualValues(snapshot);
		int value = snapshot[0];
		bool edited = false;

		if (mixed)
		{
			std::string textValue = std::to_string(value);
			edited = DrawMixedStringEditor(prop.name.c_str(), true, textValue);
			if (edited)
			{
				char* end = nullptr;
				const long parsed = std::strtol(textValue.c_str(), &end, 10);
				if (end != textValue.c_str() && *end == '\0')
					value = static_cast<int>(parsed);
				else
					edited = false;
			}
		}
		else
		{
			if (prop.hasMin && prop.hasMax)
				edited = ImGui::SliderInt(prop.name.c_str(), &value, static_cast<int>(prop.minValue), static_cast<int>(prop.maxValue));
			else
				edited = ImGui::DragInt(prop.name.c_str(), &value, prop.dragSpeed);
		}

		if (!edited)
			return false;

		if (prop.hasMin && value < static_cast<int>(prop.minValue))
			value = static_cast<int>(prop.minValue);
		if (prop.hasMax && value > static_cast<int>(prop.maxValue))
			value = static_cast<int>(prop.maxValue);

		ApplySharedValue(values, value);
		return true;
	}

	bool DrawSharedFloatProperty(const buki::PropertyInfo& prop, const std::vector<float*>& values)
	{
		if (values.empty())
			return false;

		std::vector<float> snapshot;
		snapshot.reserve(values.size());
		for (float* ptr : values)
			snapshot.push_back(*ptr);

		const bool mixed = !AreAllEqualValues(snapshot);
		float value = snapshot[0];
		bool edited = false;

		if (mixed)
		{
			char formatted[64] = {};
			std::snprintf(formatted, sizeof(formatted), "%.3f", value);
			std::string textValue = formatted;
			edited = DrawMixedStringEditor(prop.name.c_str(), true, textValue);
			if (edited)
			{
				char* end = nullptr;
				const float parsed = std::strtof(textValue.c_str(), &end);
				if (end != textValue.c_str() && *end == '\0')
					value = parsed;
				else
					edited = false;
			}
		}
		else
		{
			if (prop.hasMin && prop.hasMax)
				edited = ImGui::SliderFloat(prop.name.c_str(), &value, prop.minValue, prop.maxValue);
			else
				edited = ImGui::DragFloat(prop.name.c_str(), &value, prop.dragSpeed);
		}

		if (!edited)
			return false;

		if (prop.hasMin && value < prop.minValue)
			value = prop.minValue;
		if (prop.hasMax && value > prop.maxValue)
			value = prop.maxValue;

		ApplySharedValue(values, value);
		return true;
	}

	bool DrawSharedBoolProperty(const buki::PropertyInfo& prop, const std::vector<bool*>& values)
	{
		if (values.empty())
			return false;

		bool value = *values[0];
		bool mixed = false;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (*values[i] != value)
			{
				mixed = true;
				break;
			}
		}

		bool edited = false;
		if (mixed)
			edited = buki::InspectorPropertyDrawer::DrawCheckboxMixedFallback(prop.name.c_str(), ("##" + prop.name).c_str(), value, true);
		else
			edited = ImGui::Checkbox(prop.name.c_str(), &value);

		if (!edited)
			return false;

		ApplySharedValue(values, value);
		return true;
	}

	bool DrawSharedStringProperty(const buki::PropertyInfo& prop, const std::vector<std::string*>& values)
	{
		if (values.empty())
			return false;

		std::vector<std::string> snapshot;
		snapshot.reserve(values.size());
		for (std::string* ptr : values)
			snapshot.push_back(ptr ? *ptr : "");

		const bool mixed = !AreAllEqualValues(snapshot);
		std::string value = snapshot[0];

		if (!DrawMixedStringEditor(prop.name.c_str(), mixed, value))
			return false;

		ApplySharedValue(values, value);
		return true;
	}

	bool DrawInlineAssetPreview(const std::string& path, const char* payloadType, float thumbnailSize = 48.0f)
	{
		fs::path previewPath = path.empty() ? fs::path{} : fs::path(path);
		bool isDirectory = false;

		const buki::Texture2D* texture = nullptr;

		if (!path.empty())
		{
			if (std::strcmp(payloadType, "IMAGE") == 0)
			{
				texture = buki::GetBrowserThumbnail(previewPath, false);
			}
			else if (std::strcmp(payloadType, "AUDIO") == 0 ||
				std::strcmp(payloadType, "PREFAB") == 0)
			{
				texture = buki::GetBrowserThumbnail(previewPath, false);
			}
		}

		if (texture == nullptr || !texture->IsValid())
			texture = buki::GetBrowserThumbnail(fs::path{}, true); // fallback folder only if you want, otherwise just no draw

		ImGui::PushID(("Preview_" + path + payloadType).c_str());

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.08f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.14f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1, 1, 1, 0.10f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

		ImVec2 buttonSize(thumbnailSize, thumbnailSize);
		ImVec2 imageSize(thumbnailSize, thumbnailSize);
		ImVec2 padding(16.0f, 16.0f);
		if (texture->height != texture->width)
		{
			float aspect = static_cast<float>(texture->width) / static_cast<float>(texture->height);
			if (aspect > 1.0f)
			{
				imageSize.y /= aspect;
				padding.y += (thumbnailSize - imageSize.y) * 0.5f;
			}
			else
			{
				imageSize.x *= aspect;
				padding.x += (thumbnailSize - imageSize.x) * 0.5f;
			}
		}
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);
		bool clicked = false;

		if (texture != nullptr && texture->IsValid())
		{
			clicked = ImGui::ImageButton(
				"##inline_asset_preview",
				static_cast<ImTextureID>(texture->id),
				imageSize,
				ImVec2(0, 0),
				ImVec2(1, 1),
				ImVec4(0, 0, 0, 0),
				ImVec4(1, 1, 1, 1));
		}
		else
		{
			clicked = ImGui::Button("##inline_asset_preview_empty", buttonSize);
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(4);
		ImGui::PopID();

		return clicked;
	}

	bool DrawSharedStringAssetProperty(
		const buki::PropertyInfo& prop,
		const std::vector<std::string*>& values,
		const char* payloadType)
	{
		if (values.empty())
			return false;

		std::vector<std::string> snapshot;
		snapshot.reserve(values.size());
		for (std::string* ptr : values)
			snapshot.push_back(ptr ? *ptr : "");

		const bool mixed = !AreAllEqualValues(snapshot);
		std::string path = snapshot[0];
		bool changed = false;

		ImGui::PushID(prop.name.c_str());

		ImGui::Text("%s", prop.name.c_str());
		ImGui::SameLine(180.0f);

		std::string previewPath = mixed ? "" : path;
		if (DrawInlineAssetPreview(previewPath, payloadType, 44.0f))
			ImGui::OpenPopup("SharedAssetPickerPopup");

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadType))
			{
				const char* dropped = static_cast<const char*>(payload->Data);
				if (dropped != nullptr)
				{
					path = dropped;
					changed = true;
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();

		if (mixed)
		{
			ImGui::BeginGroup();
			ImGui::TextUnformatted("-");
			ImGui::TextDisabled("(mixed)");
			ImGui::EndGroup();
		}
		else
		{
			ImGui::BeginGroup();
			ImGui::TextUnformatted(path.empty() ? "None" : buki::DisplayNameForPath(path).c_str());
			ImGui::TextDisabled("%s", path.empty() ? "No asset selected" : fs::path(path).parent_path().generic_string().c_str());
			ImGui::EndGroup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Pick"))
			ImGui::OpenPopup("SharedAssetPickerPopup");

		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			path.clear();
			changed = true;
		}

		changed |= DrawAssetPickerPopupGrid("SharedAssetPickerPopup", payloadType, path);

		ImGui::PopID();

		if (!changed)
			return false;

		ApplySharedValue(values, path);
		return true;
	}

	bool DrawSharedStringListProperty(
		const buki::PropertyInfo& prop,
		const std::vector<std::vector<std::string>*>& values)
	{
		if (values.empty())
			return false;

		std::vector<std::string> first = values[0] ? *values[0] : std::vector<std::string>{};
		bool mixed = false;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (values[i] == nullptr || *values[i] != first)
			{
				mixed = true;
				break;
			}
		}
		ImGui::PushID(prop.name.c_str());

		std::vector<std::string> editValues = first;

		bool applyRequested = false;
		bool clearRequested = false;
		bool changedInEditor = DrawAssetListEditor(
			prop.name.c_str(),
			editValues,
			"IMAGE",
			mixed,
			applyRequested,
			clearRequested);

		bool changed = false;

		if (!mixed && changedInEditor)
		{
			for (std::vector<std::string>* ptr : values)
			{
				if (ptr != nullptr)
					*ptr = editValues;
			}
			changed = true;
		}
		else if (mixed && applyRequested)
		{
			for (std::vector<std::string>* ptr : values)
			{
				if (ptr != nullptr)
					*ptr = editValues;
			}
			changed = true;
		}
		else if (mixed && clearRequested)
		{
			for (std::vector<std::string>* ptr : values)
			{
				if (ptr != nullptr)
					ptr->clear();
			}
			changed = true;
		}

		ImGui::PopID();
		return changed;
	}

	bool DrawSharedTypedAssetListProperty(
		const buki::PropertyInfo& prop,
		const std::vector<std::vector<std::string>*>& values,
		const char* payloadType)
	{
		if (values.empty())
			return false;

		std::vector<std::string> first = values[0] ? *values[0] : std::vector<std::string>{};
		bool mixed = false;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (values[i] == nullptr || *values[i] != first)
			{
				mixed = true;
				break;
			}
		}

		ImGui::PushID(prop.name.c_str());

		std::vector<std::string> editValues = first;

		bool applyRequested = false;
		bool clearRequested = false;
		bool changedInEditor = DrawAssetListEditor(
			prop.name.c_str(),
			editValues,
			payloadType,
			mixed,
			applyRequested,
			clearRequested);

		bool changed = false;

		if (!mixed && changedInEditor)
		{
			for (std::vector<std::string>* ptr : values)
			{
				if (ptr != nullptr)
					*ptr = editValues;
			}
			changed = true;
		}
		else if (mixed && applyRequested)
		{
			for (std::vector<std::string>* ptr : values)
			{
				if (ptr != nullptr)
					*ptr = editValues;
			}

			changed = true;
		}
		else if (mixed && clearRequested)
		{
			for (std::vector<std::string>* ptr : values)
			{
				if (ptr != nullptr)
					ptr->clear();
			}
			changed = true;
		}

		ImGui::PopID();
		return changed;
	}

	bool DrawSharedEntityRefProperty(
		const buki::PropertyInfo& prop,
		const std::vector<buki::EntityRef*>& refs)
	{
		if (refs.empty())
			return false;

		const bool mixed = !AreAllEqualEntityRefs(refs);
		buki::EntityRef preview = refs[0] ? *refs[0] : buki::EntityRef{};
		bool changed = false;

		ImGui::Text("%s", prop.name.c_str());
		ImGui::SameLine(180.0f);

		std::string previewText = "None";
		if (mixed)
		{
			previewText = "-";
		}
		else if (preview.cached != nullptr)
		{
			previewText = preview.cached->GetName();
		}
		else if (!preview.entityName.empty())
		{
			previewText = preview.entityName;
		}
		else if (!preview.prefabPath.empty())
		{
			previewText = buki::DisplayNameForPath(preview.prefabPath);
		}

		if (ImGui::Button(previewText.c_str(), ImVec2(220.0f, 0.0f)))
			ImGui::OpenPopup(("SharedEntityPicker##" + prop.name).c_str());

		if (ImGui::BeginPopup(("SharedEntityPicker##" + prop.name).c_str()))
		{
			if (ImGui::Selectable("None"))
			{
				for (buki::EntityRef* ref : refs)
				{
					if (ref != nullptr)
						ref->Clear();
				}
				changed = true;
				ImGui::CloseCurrentPopup();
			}

			buki::IWorld& world = buki::Engine::Get().World();
			for (buki::Entity* entity : world.GetEntitiesInWorld())
			{
				if (entity == nullptr)
					continue;

				const std::string entityName = entity->GetName();
				if (ImGui::Selectable(entityName.c_str(), !mixed && preview.entityName == entityName && preview.prefabPath.empty()))
				{
					for (buki::EntityRef* ref : refs)
					{
						if (ref == nullptr)
							continue;

						ref->entityName = entityName;
						ref->prefabPath.clear();
						ref->cached = entity;
					}
					changed = true;
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::Separator();
			ImGui::TextDisabled("Drop an entity or prefab here.");

			ImGui::EndPopup();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
			{
				buki::Entity* droppedEntity = *static_cast<buki::Entity* const*>(payload->Data);
				if (droppedEntity != nullptr)
				{
					for (buki::EntityRef* ref : refs)
					{
						if (ref == nullptr)
							continue;

						ref->entityName = droppedEntity->GetName();
						ref->prefabPath.clear();
						ref->cached = droppedEntity;
					}
					changed = true;
				}
			}

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PREFAB"))
			{
				const char* droppedPath = static_cast<const char*>(payload->Data);
				if (droppedPath != nullptr)
				{
					for (buki::EntityRef* ref : refs)
					{
						if (ref == nullptr)
							continue;

						ref->entityName.clear();
						ref->prefabPath = droppedPath;
						ref->cached = nullptr;
					}
					changed = true;
				}
			}

			ImGui::EndDragDropTarget();
		}

		return changed;
	}

}

bool buki::InspectorPropertyDrawer::DrawComponent(Component* component, std::string* name)
{
	if (component == nullptr)
		return false;

	MonoBehaviour* behaviour = dynamic_cast<MonoBehaviour*>(component);
	if (behaviour != nullptr)
		return DrawMonoBehaviour(behaviour, name);

	return false;
}

bool buki::InspectorPropertyDrawer::DrawMonoBehaviour(MonoBehaviour* behaviour, std::string* name)
{
	if (behaviour == nullptr)
		return false;

	bool changed = false;

	std::string cmpName = ComponentFactory::GetTypeName(typeid(*behaviour));
	if (cmpName.empty())
		cmpName = "MonoBehaviour";

	if (name != nullptr)
		*name = cmpName;

	if (ImGui::TreeNodeEx(cmpName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (const PropertyInfo& prop : behaviour->GetProperties())
		{
			char* base = reinterpret_cast<char*>(behaviour);
			void* fieldPtr = base + prop.offset;

			ImGui::PushID(prop.name.c_str());
			changed |= DrawProperty(prop, fieldPtr);
			ImGui::PopID();
		}

		behaviour->OnInspectorGUI();
		ImGui::TreePop();
	}

	return changed;
}

bool buki::InspectorPropertyDrawer::DrawProperty(const PropertyInfo& prop, void* fieldPtr)
{
	switch (prop.type)
	{
	case PropertyType::Int:            return DrawIntProperty(prop, fieldPtr);
	case PropertyType::Float:          return DrawFloatProperty(prop, fieldPtr);
	case PropertyType::Bool:           return DrawBoolProperty(prop, fieldPtr);
	case PropertyType::String:         return DrawStringProperty(prop, fieldPtr);
	case PropertyType::StringList:     return DrawStringListProperty(prop, fieldPtr);
	case PropertyType::ImageAsset:     return DrawStringAssetPicker(prop, fieldPtr, "IMAGE");
	case PropertyType::AudioAsset:     return DrawStringAssetPicker(prop, fieldPtr, "AUDIO");
	case PropertyType::ImageAssetList: return DrawStringAssetListPicker(prop, fieldPtr, "IMAGE");
	case PropertyType::AudioAssetList: return DrawStringAssetListPicker(prop, fieldPtr, "AUDIO");
	case PropertyType::EntityRef:      return DrawEntityPicker(prop, fieldPtr);
	case PropertyType::PrefabRef:      return DrawPrefabPicker(prop, fieldPtr);
	default:                           return false;
	}
}

bool buki::InspectorPropertyDrawer::DrawIntProperty(const PropertyInfo& prop, void* fieldPtr)
{
	int* value = reinterpret_cast<int*>(fieldPtr);
	if (value == nullptr)
		return false;

	bool changed = false;

	if (prop.hasMin && prop.hasMax)
		changed = ImGui::SliderInt(prop.name.c_str(), value, static_cast<int>(prop.minValue), static_cast<int>(prop.maxValue));
	else
		changed = ImGui::DragInt(prop.name.c_str(), value, prop.dragSpeed);

	if (!changed)
		return false;

	if (prop.hasMin && *value < static_cast<int>(prop.minValue))
		*value = static_cast<int>(prop.minValue);
	if (prop.hasMax && *value > static_cast<int>(prop.maxValue))
		*value = static_cast<int>(prop.maxValue);

	return true;
}

bool buki::InspectorPropertyDrawer::DrawFloatProperty(const PropertyInfo& prop, void* fieldPtr)
{
	float* value = reinterpret_cast<float*>(fieldPtr);
	if (value == nullptr)
		return false;

	bool changed = false;

	if (prop.hasMin && prop.hasMax)
		changed = ImGui::SliderFloat(prop.name.c_str(), value, prop.minValue, prop.maxValue);
	else
		changed = ImGui::DragFloat(prop.name.c_str(), value, prop.dragSpeed);

	if (!changed)
		return false;

	if (prop.hasMin && *value < prop.minValue)
		*value = prop.minValue;
	if (prop.hasMax && *value > prop.maxValue)
		*value = prop.maxValue;

	return true;
}

bool buki::InspectorPropertyDrawer::DrawBoolProperty(const PropertyInfo& prop, void* fieldPtr)
{
	bool* value = reinterpret_cast<bool*>(fieldPtr);
	if (value == nullptr)
		return false;

	return ImGui::Checkbox(prop.name.c_str(), value);
}

bool buki::InspectorPropertyDrawer::DrawStringProperty(const PropertyInfo& prop, void* fieldPtr)
{
	std::string* value = reinterpret_cast<std::string*>(fieldPtr);
	if (value == nullptr)
		return false;

	char buffer[256] = {};
	strncpy_s(buffer, value->c_str(), sizeof(buffer) - 1);

	if (!ImGui::InputText(prop.name.c_str(), buffer, sizeof(buffer)))
		return false;

	*value = buffer;
	return true;
}

bool buki::InspectorPropertyDrawer::DrawStringListProperty(const PropertyInfo& prop, void* fieldPtr)
{
	std::vector<std::string>* values = reinterpret_cast<std::vector<std::string>*>(fieldPtr);
	if (values == nullptr)
		return false;

	bool changed = false;
	int removeIndex = -1;

	ImGui::Text("%s", prop.name.c_str());
	ImGui::Indent();

	for (int i = 0; i < static_cast<int>(values->size()); ++i)
	{
		ImGui::PushID(i);

		char buffer[256] = {};
		strncpy_s(buffer, (*values)[i].c_str(), sizeof(buffer) - 1);

		ImGui::SetNextItemWidth(220.0f);
		if (ImGui::InputText("##value", buffer, sizeof(buffer)))
		{
			(*values)[i] = buffer;
			changed = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("-"))
			removeIndex = i;

		ImGui::PopID();
	}

	if (removeIndex >= 0)
	{
		values->erase(values->begin() + removeIndex);
		changed = true;
	}

	if (ImGui::Button(("Add##" + prop.name).c_str()))
	{
		values->push_back("");
		changed = true;
	}

	ImGui::Unindent();
	return changed;
}

bool buki::InspectorPropertyDrawer::DrawStringAssetPicker(const PropertyInfo& prop, void* fieldPtr, const char* payloadType)
{
	std::string* path = reinterpret_cast<std::string*>(fieldPtr);
	if (path == nullptr)
		return false;

	bool changed = false;

	ImGui::PushID(prop.name.c_str());

	ImGui::Text("%s", prop.name.c_str());
	ImGui::SameLine(180.0f);

	if (DrawInlineAssetPreview(*path, payloadType, 44.0f))
		ImGui::OpenPopup("AssetPickerPopup");

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadType))
		{
			const char* dropped = static_cast<const char*>(payload->Data);
			*path = dropped ? dropped : "";
			changed = true;
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();

	const std::string displayName = path->empty() ? "None" : DisplayNameForPath(*path);
	ImGui::BeginGroup();
	ImGui::TextUnformatted(displayName.c_str());

	if (!path->empty())
	{
		ImGui::TextDisabled("%s", fs::path(*path).parent_path().generic_string().c_str());
	}
	else
	{
		ImGui::TextDisabled("No asset selected");
	}
	ImGui::EndGroup();

	ImGui::SameLine();
	if (ImGui::Button("Pick"))
		ImGui::OpenPopup("AssetPickerPopup");

	ImGui::SameLine();
	if (ImGui::Button("Clear"))
	{
		path->clear();
		changed = true;
	}

	changed |= DrawAssetPickerPopupGrid("AssetPickerPopup", payloadType, *path);

	ImGui::PopID();
	return changed;
}

bool buki::InspectorPropertyDrawer::DrawStringAssetListPicker(const PropertyInfo& prop, void* fieldPtr, const char* payloadType)
{
	std::vector<std::string>* values = reinterpret_cast<std::vector<std::string>*>(fieldPtr);
	if (values == nullptr)
		return false;

	ImGui::PushID(prop.name.c_str());

	bool applyRequested = false;
	bool clearRequested = false;
	bool changed = DrawAssetListEditor(prop.name.c_str(), *values, payloadType, false, applyRequested, clearRequested);

	if (clearRequested)
	{
		values->clear();
		changed = true;
	}


	ImGui::PopID();
	return changed;
}

bool buki::InspectorPropertyDrawer::DrawSpriteTexturePathField(const char* label, std::string& texturePath)
{
	bool changed = false;
	ImGui::PushID(label);

	ImGui::Text("%s :", label);
	//ImGui::SameLine(180.0f);

	if (DrawInlineAssetPreview(texturePath, "IMAGE", 44.0f))
		ImGui::OpenPopup("SpriteTexturePicker");

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMAGE"))
		{
			const char* dropped = static_cast<const char*>(payload->Data);
			if (dropped != nullptr)
			{
				texturePath = dropped;
				changed = true;
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();

	const std::string displayName = texturePath.empty() ? "None" : DisplayNameForPath(texturePath);
	ImGui::BeginGroup();
	ImGui::TextUnformatted(displayName.c_str());
	ImGui::TextDisabled("%s",
		texturePath.empty() ? "No image selected" : fs::path(texturePath).parent_path().generic_string().c_str());
	ImGui::EndGroup();

	ImGui::SameLine();
	if (ImGui::Button("Pick"))
		ImGui::OpenPopup("SpriteTexturePicker");

	ImGui::SameLine();
	if (ImGui::Button("Clear"))
	{
		texturePath.clear();
		changed = true;
	}

	changed |= DrawAssetPickerPopupGrid("SpriteTexturePicker", "IMAGE", texturePath);

	ImGui::PopID();
	return changed;
}

bool buki::InspectorPropertyDrawer::DrawEntityPicker(const PropertyInfo& prop, void* fieldPtr)
{
	EntityRef* ref = reinterpret_cast<EntityRef*>(fieldPtr);
	if (ref == nullptr)
		return false;

	bool changed = false;

	ImGui::Text("%s", prop.name.c_str());
	ImGui::SameLine(180.0f);

	std::string previewText = "None";
	if (ref->cached != nullptr)
		previewText = ref->cached->GetName();
	else if (!ref->entityName.empty())
		previewText = ref->entityName;
	else if (!ref->prefabPath.empty())
		previewText = DisplayNameForPath(ref->prefabPath);

	if (ImGui::Button(previewText.c_str(), ImVec2(220.0f, 0.0f)))
		ImGui::OpenPopup(("EntityPicker##" + prop.name).c_str());

	if (ImGui::BeginPopup(("EntityPicker##" + prop.name).c_str()))
	{
		if (ImGui::Selectable("None"))
		{
			ref->Clear();
			changed = true;
		}

		IWorld& world = Engine::Get().World();
		for (Entity* entity : world.GetEntitiesInWorld())
		{
			if (entity == nullptr)
				continue;

			const std::string entityName = entity->GetName();
			const bool selected = (ref->entityName == entityName && ref->prefabPath.empty());

			if (ImGui::Selectable(entityName.c_str(), selected))
			{
				ref->entityName = entityName;
				ref->prefabPath.clear();
				ref->cached = entity;
				changed = true;
			}
		}

		ImGui::Separator();
		ImGui::TextDisabled("Drop an entity or prefab here.");

		ImGui::EndPopup();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
		{
			Entity* droppedEntity = *static_cast<Entity* const*>(payload->Data);
			if (droppedEntity != nullptr)
			{
				ref->entityName = droppedEntity->GetName();
				ref->prefabPath.clear();
				ref->cached = droppedEntity;
				changed = true;
			}
		}

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PREFAB"))
		{
			const char* droppedPath = static_cast<const char*>(payload->Data);
			if (droppedPath != nullptr)
			{
				ref->entityName.clear();
				ref->prefabPath = droppedPath;
				ref->cached = nullptr;
				changed = true;
			}
		}

		ImGui::EndDragDropTarget();
	}

	return changed;
}

bool buki::InspectorPropertyDrawer::DrawPrefabPicker(const PropertyInfo& prop, void* fieldPtr)
{
	return DrawStringAssetPicker(prop, fieldPtr, "PREFAB");
}

bool buki::InspectorPropertyDrawer::DrawSharedComponent(const std::string& componentTypeName, const std::vector<Entity*>& entities)
{
	if (entities.empty())
		return false;

	for (Entity* entity : entities)
	{
		if (entity == nullptr)
			return false;
	}

	Component* first = entities[0]->GetComponentByTypeName(componentTypeName);
	if (first == nullptr)
		return false;

	MonoBehaviour* firstBehaviour = dynamic_cast<MonoBehaviour*>(first);
	if (firstBehaviour == nullptr)
	{
		ImGui::TextDisabled("Multi-edit currently implemented for MonoBehaviour/reflected components.");
		return false;
	}

	bool changed = false;

	for (const PropertyInfo& prop : firstBehaviour->GetProperties())
	{
		switch (prop.type)
		{
		case PropertyType::Int:
			changed |= DrawSharedIntProperty(prop, GatherSharedFieldPtrs<int>(componentTypeName, entities, prop.offset));
			break;

		case PropertyType::Float:
			changed |= DrawSharedFloatProperty(prop, GatherSharedFieldPtrs<float>(componentTypeName, entities, prop.offset));
			break;

		case PropertyType::Bool:
			changed |= DrawSharedBoolProperty(prop, GatherSharedFieldPtrs<bool>(componentTypeName, entities, prop.offset));
			break;

		case PropertyType::String:
			changed |= DrawSharedStringProperty(prop, GatherSharedFieldPtrs<std::string>(componentTypeName, entities, prop.offset));
			break;

		case PropertyType::ImageAsset:
			changed |= DrawSharedStringAssetProperty(prop, GatherSharedFieldPtrs<std::string>(componentTypeName, entities, prop.offset), "IMAGE");
			break;

		case PropertyType::AudioAsset:
			changed |= DrawSharedStringAssetProperty(prop, GatherSharedFieldPtrs<std::string>(componentTypeName, entities, prop.offset), "AUDIO");
			break;

		case PropertyType::PrefabRef:
			changed |= DrawSharedStringAssetProperty(prop, GatherSharedFieldPtrs<std::string>(componentTypeName, entities, prop.offset), "PREFAB");
			break;

		case PropertyType::StringList:
			changed |= DrawSharedStringListProperty(prop, GatherSharedFieldPtrs<std::vector<std::string>>(componentTypeName, entities, prop.offset));
			break;

		case PropertyType::ImageAssetList:
			changed |= DrawSharedTypedAssetListProperty(prop, GatherSharedFieldPtrs<std::vector<std::string>>(componentTypeName, entities, prop.offset), "IMAGE");

			break;

		case PropertyType::AudioAssetList:
			changed |= DrawSharedTypedAssetListProperty(prop, GatherSharedFieldPtrs<std::vector<std::string>>(componentTypeName, entities, prop.offset), "AUDIO");
			break;

		case PropertyType::EntityRef:
			changed |= DrawSharedEntityRefProperty(prop, GatherSharedFieldPtrs<EntityRef>(componentTypeName, entities, prop.offset));
			break;

		default:
			ImGui::TextDisabled("%s: unsupported property type", prop.name.c_str());
			break;
		}
	}

	return changed;
}

bool buki::InspectorPropertyDrawer::DrawCheckboxMixedFallback(const char* label, const char* hiddenId, bool& value, bool mixed)
{
	if (mixed)
	{
		ImGui::TextUnformatted(label);
		ImGui::SameLine();
		ImGui::TextDisabled("(mixed)");
		return ImGui::Checkbox(hiddenId, &value);
	}

	return ImGui::Checkbox(label, &value);
}

bool buki::InspectorPropertyDrawer::DrawAddComponentPopup(Entity* owner)
{
	if (owner == nullptr)
		return false;

	bool changed = false;

	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponentPopup");

	if (ImGui::BeginPopup("AddComponentPopup"))
	{
		if (ImGui::BeginMenu("Registered Components"))
		{
			for (const std::string& typeName : ComponentFactory::GetRegisteredTypeNames())
			{
				if (ImGui::MenuItem(typeName.c_str()))
				{
					if (!owner->HasComponent(typeName))
					{
						owner->AddComponentByTypeName(typeName);
						changed = true;
					}
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}

	return changed;
}

bool buki::InspectorPropertyDrawer::DrawAddComponentPopup(const std::vector<Entity*>& owners)
{
	if (owners.empty())
		return false;

	bool changed = false;

	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddSharedComponentPopup");

	if (ImGui::BeginPopup("AddSharedComponentPopup"))
	{
		if (ImGui::BeginMenu("Registered Components"))
		{
			for (const std::string& typeName : ComponentFactory::GetRegisteredTypeNames())
			{
				if (ImGui::MenuItem(typeName.c_str()))
				{
					for (Entity* owner : owners)
					{
						if (owner != nullptr && !owner->HasComponent(typeName))
						{
							owner->AddComponentByTypeName(typeName);
							changed = true;
						}
					}
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}

	return changed;
}

std::vector<std::string> buki::InspectorPropertyDrawer::GetSharedComponentTypeNames(const std::vector<Entity*>& entities)
{
	std::vector<std::string> result;

	if (entities.empty() || entities[0] == nullptr)
		return result;

	for (auto& [type, component] : entities[0]->GetComponents())
	{
		if (component == nullptr)
			continue;

		std::string typeName = ComponentFactory::GetTypeName(*type);
		if (typeName.empty())
			typeName = type->name();

		bool existsOnAll = true;
		for (size_t i = 1; i < entities.size(); ++i)
		{
			Entity* entity = entities[i];
			if (entity == nullptr || !entity->HasComponent(typeName))
			{
				existsOnAll = false;
				break;
			}
		}

		if (existsOnAll)
			result.push_back(typeName);
	}

	return result;
}

void buki::InspectorPropertyDrawer::ClearCachedEditorState()
{
	sMixedTextStates.clear();
	sListEditStates.clear();
}
