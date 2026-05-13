#include "InspectorPropertyDrawer.h"
#include "Component.h"
#include "MonoBehaviour.h"
#include "Engine.h"
#include "Entity.h"
#include "IWorld.h"
#include "PropertyInfo.h"
#include "imgui.h"
#include "ComponentFactory.h"
#include "EntityRef.h"

bool buki::InspectorPropertyDrawer::DrawComponent(Component* component, std::string* name)
{
	if (component == nullptr)
	{
		return false;
	}

	MonoBehaviour* behaviour = dynamic_cast<MonoBehaviour*>(component);
	if (behaviour != nullptr)
	{
		return DrawMonoBehaviour(behaviour, name);
	}

	return false;
}

bool buki::InspectorPropertyDrawer::DrawMonoBehaviour(MonoBehaviour* behaviour, std::string* name)
{
	if (behaviour == nullptr)
	{
		return false;
	}

	bool changed = false;
	std::string cmpName = ComponentFactory::GetTypeName(typeid(*behaviour));
	*name = cmpName;
	if (cmpName.empty())
	{
		cmpName = "MonoBehaviour";
	}

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
		ImGui::TreePop();
	}

	behaviour->OnInspectorGUI();
	return changed;
}

bool buki::InspectorPropertyDrawer::DrawProperty(const PropertyInfo& prop, void* fieldPtr)
{
	switch (prop.type)
	{
	case PropertyType::Int:
		return DrawIntProperty(prop, fieldPtr);
	case PropertyType::Float:
		return DrawFloatProperty(prop, fieldPtr);
	case PropertyType::Bool:
		return DrawBoolProperty(prop, fieldPtr);
	case PropertyType::String:
		return DrawStringProperty(prop, fieldPtr);
	case PropertyType::StringList:
		return DrawStringListProperty(prop, fieldPtr);
	case PropertyType::ImageAsset:
		return DrawAssetPicker(prop, fieldPtr, "IMAGE");
	case PropertyType::AudioAsset:
		return DrawAssetPicker(prop, fieldPtr, "AUDIO");
	case PropertyType::EntityRef:
		return DrawEntityPicker(prop, fieldPtr);
	case PropertyType::PrefabRef:
		return DrawPrefabPicker(prop, fieldPtr);
	default:
		return false;
	}
}

bool buki::InspectorPropertyDrawer::DrawIntProperty(const PropertyInfo& prop, void* fieldPtr)
{
	int* value = reinterpret_cast<int*>(fieldPtr);
	if (prop.hasMin && prop.hasMax)
		return ImGui::SliderInt(prop.name.c_str(), value, static_cast<int>(prop.minValue), static_cast<int>(prop.maxValue));
	return ImGui::DragInt(prop.name.c_str(), value, prop.dragSpeed);
}

bool buki::InspectorPropertyDrawer::DrawFloatProperty(const PropertyInfo& prop, void* fieldPtr)
{
	float* value = reinterpret_cast<float*>(fieldPtr);
	if (prop.hasMin && prop.hasMax)
		return ImGui::SliderFloat(prop.name.c_str(), value, prop.minValue, prop.maxValue);
	return ImGui::DragFloat(prop.name.c_str(), value, prop.dragSpeed);
}

bool buki::InspectorPropertyDrawer::DrawBoolProperty(const PropertyInfo& prop, void* fieldPtr)
{
	bool* value = reinterpret_cast<bool*>(fieldPtr);
	return ImGui::Checkbox(prop.name.c_str(), value);
}

bool buki::InspectorPropertyDrawer::DrawStringProperty(const PropertyInfo& prop, void* fieldPtr)
{
	std::string* value = reinterpret_cast<std::string*>(fieldPtr);
	char buffer[256] = {};
	strncpy_s(buffer, value->c_str(), sizeof(buffer) - 1);

	if (ImGui::InputText(prop.name.c_str(), buffer, sizeof(buffer)))
	{
		*value = buffer;
		return true;
	}
	return false;
}

bool buki::InspectorPropertyDrawer::DrawStringListProperty(const PropertyInfo& prop, void* fieldPtr)
{
	std::vector<std::string>* values = reinterpret_cast<std::vector<std::string>*>(fieldPtr);

	ImGui::Text("%s", prop.name.c_str());
	ImGui::Indent();

	bool changed = false;
	int removeIndex = -1;

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
		{
			removeIndex = i;
		}

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

bool buki::InspectorPropertyDrawer::DrawAssetPicker(const PropertyInfo& prop, void* fieldPtr, const char* payloadType)
{
	std::string* path = reinterpret_cast<std::string*>(fieldPtr);
	bool changed = false;

	ImGui::Text("%s", prop.name.c_str());
	ImGui::SameLine(180.0f);

	char buffer[256] = {};
	strncpy_s(buffer, path->c_str(), sizeof(buffer) - 1);

	ImGui::SetNextItemWidth(220.0f);
	if (ImGui::InputText(("##" + prop.name).c_str(), buffer, sizeof(buffer)))
	{
		*path = buffer;
		changed = true;
	}

	ImGui::SameLine();
	if (ImGui::Button(("...##" + prop.name).c_str()))
	{
		ImGui::OpenPopup(("AssetPicker##" + prop.name).c_str());
	}

	if (ImGui::BeginPopup(("AssetPicker##" + prop.name).c_str()))
	{
		ImGui::Text("Drop %s asset here", payloadType);
		ImGui::Separator();

		if (ImGui::Selectable("Clear"))
		{
			path->clear();
			changed = true;
		}

		ImGui::EndPopup();
	}

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
	{
		previewText = ref->cached->GetName();
	}
	else if (!ref->entityName.empty())
	{
		previewText = ref->entityName;
	}
	else if (!ref->prefabPath.empty())
	{
		previewText = ref->prefabPath;
	}

	if (ImGui::Button(previewText.c_str(), ImVec2(220.0f, 0.0f)))
	{
		ImGui::OpenPopup(("EntityPicker##" + prop.name).c_str());
	}

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

			std::string name = entity->GetName();
			const bool selected = (ref->entityName == name);

			if (ImGui::Selectable(name.c_str(), selected))
			{
				ref->entityName = name;
				ref->prefabPath.clear();
				ref->cached = entity;
				changed = true;
			}
		}

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
	std::string* prefabPath = reinterpret_cast<std::string*>(fieldPtr);
	bool changed = false;

	ImGui::Text("%s", prop.name.c_str());
	ImGui::SameLine(180.0f);

	char buffer[256] = {};
	strncpy_s(buffer, prefabPath->c_str(), sizeof(buffer) - 1);

	ImGui::SetNextItemWidth(220.0f);
	if (ImGui::InputText(("##" + prop.name).c_str(), buffer, sizeof(buffer)))
	{
		*prefabPath = buffer;
		changed = true;
	}

	ImGui::SameLine();
	if (ImGui::Button(("...##" + prop.name).c_str()))
	{
		ImGui::OpenPopup(("PrefabPicker##" + prop.name).c_str());
	}

	if (ImGui::BeginPopup(("PrefabPicker##" + prop.name).c_str()))
	{
		ImGui::Text("Choose a prefab");
		ImGui::Separator();

		if (ImGui::Selectable("Clear"))
		{
			prefabPath->clear();
			changed = true;
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PREFAB"))
		{
			const char* dropped = static_cast<const char*>(payload->Data);
			*prefabPath = dropped ? dropped : "";
			changed = true;
		}
		ImGui::EndDragDropTarget();
	}

	return changed;
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

std::vector<std::string> buki::InspectorPropertyDrawer::GetSharedComponentTypeNames(const std::vector<Entity*>& entities)
{
	std::vector<std::string> result;
	if (entities.empty() || entities[0] == nullptr)
	{
		return result;
	}

	for (auto& [type, component] : entities[0]->GetComponents())
	{
		if (component == nullptr)
		{
			continue;
		}

		std::string typeName = ComponentFactory::GetTypeName(*type);
		if (typeName.empty())
		{
			typeName = type->name();
		}

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
		{
			result.push_back(typeName);
		}
	}

	return result;
}

bool buki::InspectorPropertyDrawer::DrawSharedComponent(const std::string& componentTypeName, const std::vector<Entity*>& entities)
{
	if (entities.empty())
	{
		return false;
	}

	bool changed = false;

	for (Entity* entity : entities)
	{
		if (entity == nullptr)
		{
			return false;
		}
	}

	Component* first = entities[0]->GetComponentByTypeName(componentTypeName);
	if (first == nullptr)
	{
		return false;
	}

	if (dynamic_cast<MonoBehaviour*>(first) == nullptr)
	{
		ImGui::TextDisabled("Multi-edit currently implemented for MonoBehaviour/reflected components.");
		return false;
	}

	MonoBehaviour* firstBehaviour = dynamic_cast<MonoBehaviour*>(first);
	if (firstBehaviour == nullptr)
	{
		return false;
	}

	for (const PropertyInfo& prop : firstBehaviour->GetProperties())
	{
		if (prop.type == PropertyType::StringList)
		{
			ImGui::TextDisabled("%s: multi-edit not supported", prop.name.c_str());
			continue;
		}

		if (prop.type == PropertyType::EntityRef || prop.type == PropertyType::PrefabRef)
		{
			ImGui::TextDisabled("%s: multi-edit not supported yet", prop.name.c_str());
			continue;
		}

		Component* firstComponent = entities[0]->GetComponentByTypeName(componentTypeName);
		if (firstComponent == nullptr)
		{
			continue;
		}

		char* firstBase = reinterpret_cast<char*>(firstComponent);
		void* firstFieldPtr = firstBase + prop.offset;

		if (prop.type == PropertyType::Int)
		{
			int firstValue = *reinterpret_cast<int*>(firstFieldPtr);
			bool mixed = false;

			for (size_t i = 1; i < entities.size(); ++i)
			{
				Component* component = entities[i]->GetComponentByTypeName(componentTypeName);
				if (component == nullptr)
				{
					continue;
				}

				char* base = reinterpret_cast<char*>(component);
				void* fieldPtr = base + prop.offset;
				if (*reinterpret_cast<int*>(fieldPtr) != firstValue)
				{
					mixed = true;
					break;
				}
			}

			int value = firstValue;
			bool edited = false;

			if (mixed)
			{
				ImGui::TextUnformatted(prop.name.c_str());
				ImGui::SameLine();
				ImGui::TextDisabled("(mixed)");
				edited = ImGui::DragInt(("##" + prop.name).c_str(), &value, prop.dragSpeed);
			}
			else
			{
				edited = DrawIntProperty(prop, &value);
			}

			if (edited)
			{
				for (Entity* entity : entities)
				{
					Component* component = entity->GetComponentByTypeName(componentTypeName);
					if (component == nullptr)
					{
						continue;
					}

					char* base = reinterpret_cast<char*>(component);
					void* fieldPtr = base + prop.offset;
					*reinterpret_cast<int*>(fieldPtr) = value;
				}
				changed = true;
			}
		}
		else if (prop.type == PropertyType::Float)
		{
			float firstValue = *reinterpret_cast<float*>(firstFieldPtr);
			bool mixed = false;

			for (size_t i = 1; i < entities.size(); ++i)
			{
				Component* component = entities[i]->GetComponentByTypeName(componentTypeName);
				if (component == nullptr)
				{
					continue;
				}

				char* base = reinterpret_cast<char*>(component);
				void* fieldPtr = base + prop.offset;
				if (*reinterpret_cast<float*>(fieldPtr) != firstValue)
				{
					mixed = true;
					break;
				}
			}

			float value = firstValue;
			bool edited = false;

			if (mixed)
			{
				ImGui::TextUnformatted(prop.name.c_str());
				ImGui::SameLine();
				ImGui::TextDisabled("(mixed)");
				edited = ImGui::DragFloat(("##" + prop.name).c_str(), &value, prop.dragSpeed);
			}
			else
			{
				edited = DrawFloatProperty(prop, &value);
			}

			if (edited)
			{
				for (Entity* entity : entities)
				{
					Component* component = entity->GetComponentByTypeName(componentTypeName);
					if (component == nullptr)
					{
						continue;
					}

					char* base = reinterpret_cast<char*>(component);
					void* fieldPtr = base + prop.offset;
					*reinterpret_cast<float*>(fieldPtr) = value;
				}
				changed = true;
			}
		}
		else if (prop.type == PropertyType::Bool)
		{
			bool firstValue = *reinterpret_cast<bool*>(firstFieldPtr);
			bool allSame = true;

			for (size_t i = 1; i < entities.size(); ++i)
			{
				Component* component = entities[i]->GetComponentByTypeName(componentTypeName);
				if (component == nullptr)
				{
					continue;
				}

				char* base = reinterpret_cast<char*>(component);
				void* fieldPtr = base + prop.offset;
				if (*reinterpret_cast<bool*>(fieldPtr) != firstValue)
				{
					allSame = false;
					break;
				}
			}

			bool value = firstValue;
			bool edited = false;

			if (!allSame)
			{
				edited = DrawCheckboxMixedFallback(prop.name.c_str(), ("##" + prop.name).c_str(), value, !allSame);
			}
			else
			{
				edited = ImGui::Checkbox(prop.name.c_str(), &value);
			}

			if (edited)
			{
				for (Entity* entity : entities)
				{
					Component* component = entity->GetComponentByTypeName(componentTypeName);
					if (component == nullptr)
					{
						continue;
					}

					char* base = reinterpret_cast<char*>(component);
					void* fieldPtr = base + prop.offset;
					*reinterpret_cast<bool*>(fieldPtr) = value;
				}
				changed = true;
			}
		}
		else if (prop.type == PropertyType::String)
		{
			std::string firstValue = *reinterpret_cast<std::string*>(firstFieldPtr);
			bool mixed = false;

			for (size_t i = 1; i < entities.size(); ++i)
			{
				Component* component = entities[i]->GetComponentByTypeName(componentTypeName);
				if (component == nullptr)
				{
					continue;
				}

				char* base = reinterpret_cast<char*>(component);
				void* fieldPtr = base + prop.offset;
				if (*reinterpret_cast<std::string*>(fieldPtr) != firstValue)
				{
					mixed = true;
					break;
				}
			}

			char buffer[256] = {};
			strncpy_s(buffer, firstValue.c_str(), sizeof(buffer) - 1);

			if (mixed)
			{
				ImGui::TextUnformatted(prop.name.c_str());
				ImGui::SameLine();
				ImGui::TextDisabled("(mixed)");
			}

			if (ImGui::InputText((mixed ? "##" + prop.name : prop.name).c_str(), buffer, sizeof(buffer)))
			{
				for (Entity* entity : entities)
				{
					Component* component = entity->GetComponentByTypeName(componentTypeName);
					if (component == nullptr)
					{
						continue;
					}

					char* base = reinterpret_cast<char*>(component);
					void* fieldPtr = base + prop.offset;
					*reinterpret_cast<std::string*>(fieldPtr) = buffer;
				}
				changed = true;
			}
		}
		else
		{
			ImGui::TextDisabled("%s: multi-edit not supported", prop.name.c_str());
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

bool buki::InspectorPropertyDrawer::DrawAddComponentPopup(const std::vector<Entity*>& owners)
{
	if (owners.empty())
	{
		return false;
	}

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