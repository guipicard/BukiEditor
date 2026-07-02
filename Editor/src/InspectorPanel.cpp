#pragma once 
#include "InspectorPanel.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "Entity.h"
#include "BukiContainers.h"
#include "EditorState.h"
#include "Text.h"
#include "Color.h"
#include "Button.h"
#include "Engine.h"
#include "Sprite.h"
#include "TileLayer.h"
#include "RigidBody.h"
#include "Box.h"
#include "Circle.h"
#include "Polygon.h"
#include "PhysicsDefs.h"
#include "MonoBehaviour.h"
#include "InspectorPropertyDrawer.h"
#include "ComponentFactory.h"
#include "PropertyInfo.h"

#include "EditorAssetEntries.h"
#include "EditorViewportHelpers.h"

#include <fstream>
#include <filesystem>
#include "nlohmann/json.hpp"
#include <sstream>
#include <glad/glad.h>

namespace fs = std::filesystem;

namespace
{
	static bool ContainsEntityPtr(const std::vector<buki::Entity*>& entities, buki::Entity* entity)
	{
		return std::find(entities.begin(), entities.end(), entity) != entities.end();
	}

	void SanitizeSelection(buki::EditorState& state, const std::vector<buki::Entity*>& allEntities)
	{
		std::vector<buki::Entity*> filtered;
		filtered.reserve(state.selectedEntities.size());

		for (buki::Entity* entity : state.selectedEntities)
		{
			if (entity != nullptr && ContainsEntityPtr(allEntities, entity))
				filtered.push_back(entity);
		}

		state.selectedEntities = std::move(filtered);

		if (state.selectedEntity != nullptr && !ContainsEntityPtr(allEntities, state.selectedEntity))
			state.selectedEntity = nullptr;

		if (state.activeEntity != nullptr && !ContainsEntityPtr(allEntities, state.activeEntity))
			state.activeEntity = nullptr;

		if (state.selectedEntities.empty())
		{
			state.selectedEntity = nullptr;
			state.activeEntity = nullptr;
		}
		else if (state.selectedEntity == nullptr)
		{
			state.selectedEntity = state.selectedEntities.front();
			state.activeEntity = state.selectedEntity;
		}
	}

}

namespace
{
	nlohmann::json LoadJsonFileSafe(const fs::path& path)
	{
		std::ifstream in(path);
		if (!in.is_open())
			return {};

		nlohmann::json j;
		try
		{
			in >> j;
		}
		catch (...)
		{
			return {};
		}

		return j;
	}

	std::string JsonValueToDisplayString(const nlohmann::json& value)
	{
		if (value.is_string())
			return value.get<std::string>();

		if (value.is_boolean())
			return value.get<bool>() ? "true" : "false";

		if (value.is_number_integer())
			return std::to_string(value.get<int>());

		if (value.is_number_unsigned())
			return std::to_string(value.get<unsigned int>());

		if (value.is_number_float())
		{
			std::ostringstream oss;
			oss << value.get<double>();
			return oss.str();
		}

		if (value.is_null())
			return "null";

		if (value.is_array())
			return "[...]";

		if (value.is_object())
			return "{...}";

		return value.dump();
	}

	void DrawJsonObjectReadonly(const nlohmann::json& object)
	{
		if (!object.is_object())
			return;

		for (auto it = object.begin(); it != object.end(); ++it)
		{
			const std::string key = it.key();
			const nlohmann::json& value = it.value();

			if (value.is_object())
			{
				if (ImGui::TreeNode(key.c_str()))
				{
					DrawJsonObjectReadonly(value);
					ImGui::TreePop();
				}
			}
			else if (value.is_array())
			{
				if (ImGui::TreeNode((key + "[]").c_str()))
				{
					for (size_t i = 0; i < value.size(); ++i)
					{
						const nlohmann::json& item = value[i];
						const std::string label = "[" + std::to_string(i) + "]";

						if (item.is_object())
						{
							if (ImGui::TreeNode(label.c_str()))
							{
								DrawJsonObjectReadonly(item);
								ImGui::TreePop();
							}
						}
						else
						{
							ImGui::BulletText("%s: %s", label.c_str(), JsonValueToDisplayString(item).c_str());
						}
					}
					ImGui::TreePop();
				}
			}
			else
			{
				ImGui::BulletText("%s: %s", key.c_str(), JsonValueToDisplayString(value).c_str());
			}
		}
	}
}

void buki::InspectorPanel::Render(EditorState& state)
{
	ImGui::Begin("Inspector", &state.showInspector);

	auto& world = Engine::Get().World();

	if (state.selectedPrefabEntity != nullptr)
	{
		RenderPrefabInspector(state, world);
		ImGui::End();
		return;
	}

	const std::vector<Entity*> allEntities = world.GetEntitiesInWorld();
	SanitizeSelection(state, allEntities);

	std::vector<Entity*> selected;
	selected.reserve(state.selectedEntities.size());

	for (Entity* entity : state.selectedEntities)
	{
		if (entity != nullptr && ContainsEntityPtr(allEntities, entity))
			selected.push_back(entity);
	}

	if (selected.empty() &&
		state.selectedEntity != nullptr &&
		ContainsEntityPtr(allEntities, state.selectedEntity))
	{
		selected.push_back(state.selectedEntity);
		state.selectedEntities = selected;
		state.activeEntity = state.selectedEntity;
	}

	if (selected.empty())
	{
		ImGui::TextUnformatted("No entity selected.");
		ImGui::End();
		return;
	}

	if (selected.size() == 1)
	{
		RenderSingleEntityInspector(selected[0], state, world);
		ImGui::End();
		return;
	}

	RenderMultiEntityInspector(selected, state, world);
	ImGui::End();
}

void buki::InspectorPanel::RenderSingleEntityInspector(Entity* entity, EditorState& state, IWorld& world)
{
	if (entity == nullptr)
	{
		ImGui::TextUnformatted("No entity selected.");
		return;
	}

	state.selectedEntity = entity;
	state.activeEntity = entity;

	bool changed = false;

	ImGui::Text("Entity: %s", entity->GetName().c_str());

	ImGui::SameLine();
	if (ImGui::Button("Save Prefab"))
	{
		changed |= SaveSelectedEntityAsPrefab(entity);
	}
	ImGui::Separator();

	changed |= InspectorPropertyDrawer().DrawEntitySection(entity);

	std::string componentToRemove;

	if (ImGui::CollapsingHeader("Components", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto& [type, component] : entity->GetComponents())
		{
			if (component == nullptr) continue;

			std::string cmpName = ComponentFactory::GetTypeName(*type);
			if (cmpName.empty()) cmpName = type->name();

			changed |= InspectorPropertyDrawer::DrawComponent(component, &cmpName);

			if (ImGui::Button(("Remove##" + cmpName).c_str())) componentToRemove = cmpName;
		}
	}

	if (changed) entity->Set();
	if (!componentToRemove.empty()) changed |= entity->RemoveComponentByTypeName(componentToRemove);
	changed |= InspectorPropertyDrawer::DrawAddComponentPopup(entity);


	if (changed)
	{
		state.MarkSceneDirty();
	}
}

void buki::InspectorPanel::RenderMultiEntityInspector(const std::vector<Entity*>& selected, EditorState& state, IWorld& world)
{
	if (selected.empty())
	{
		ImGui::TextUnformatted("No entity selected.");
		return;
	}

	state.selectedEntity = state.activeEntity != nullptr ? state.activeEntity : selected.front();

	bool changed = false;

	ImGui::Text("Entities Selected: %d", static_cast<int>(selected.size()));

	ImGui::Separator();

	InspectorPropertyDrawer drawer;
	changed |= drawer.DrawMultiEntitySection(selected);

	if (ImGui::CollapsingHeader("Shared Components", ImGuiTreeNodeFlags_DefaultOpen))
	{
		changed |= InspectorPropertyDrawer::DrawAddComponentPopup(selected);

		const std::vector<std::string> sharedComponentTypes =
			InspectorPropertyDrawer::GetSharedComponentTypeNames(selected);

		std::string componentToRemove;

		for (const std::string& typeName : sharedComponentTypes)
		{
			if (ImGui::TreeNodeEx(typeName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				changed |= InspectorPropertyDrawer::DrawSharedComponent(typeName, selected);

				if (ImGui::Button(("Remove##" + typeName).c_str()))
					componentToRemove = typeName;

				ImGui::TreePop();
			}
		}

		if (!componentToRemove.empty())
		{
			for (Entity* entity : selected)
			{
				if (entity != nullptr)
					changed |= entity->RemoveComponentByTypeName(componentToRemove);
			}
		}
	}

	if (changed)
	{
		for (auto& e : selected)
		{
			e->Set();
		}
		state.MarkSceneDirty();
	}
}

std::string buki::InspectorPanel::RemoveComponent(Component* cmp)
{
	std::string name = ComponentFactory::GetTypeName(typeid(cmp));
	if (!ImGui::Button(("Remove " + name).c_str()))
		return "";
	return name;
}

bool buki::InspectorPanel::SaveSelectedEntityAsPrefab(Entity* entity)
{
	if (entity == nullptr)
		return false;

	std::filesystem::path dir = std::filesystem::path("../Deployment/Prefabs");
	std::error_code ec;
	std::filesystem::create_directories(dir, ec);

	std::string fileName = entity->GetName();
	if (fileName.empty())
		fileName = "Prefab";

	for (char& c : fileName)
	{
		if (c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|')
			c = '_';
	}

	std::filesystem::path path = dir / (fileName + ".prefab");
	std::ofstream out(path);
	if (!out.is_open())
		return false;

	out << entity->Serialize().dump(4);
	return true;
}


void buki::InspectorPanel::DrawPrefabPreviewWindows(EditorState& state)
{
	for (size_t i = 0; i < state.prefabPreviewSessions.size();)
	{
		auto& session = state.prefabPreviewSessions[i];
		bool open = session.open;

		std::string title =
			buki::GetFileName(session.path) + "##PrefabPreview_" + session.windowId;

		if (session.requestDockNextToScene && state.prefabDockNodeId != 0)
		{
			ImGui::SetNextWindowDockID(state.prefabDockNodeId, ImGuiCond_Always);
			session.requestDockNextToScene = false;
		}

		if (session.requestFocus)
		{
			ImGui::SetNextWindowFocus();
		}

		if (ImGui::Begin(title.c_str(), &open))
		{
			if (session.requestFocus)
			{
				ImGui::FocusWindow(ImGui::GetCurrentWindow());
			}

			const bool focusedNow =
				session.requestFocus ||
				ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) ||
				ImGui::IsWindowAppearing();

			if (focusedNow)
			{
				for (size_t j = 0; j < state.prefabPreviewSessions.size(); ++j)
				{
					state.prefabPreviewSessions[j].focused = (j == i);
				}

				state.activePrefabPreviewIndex = static_cast<int>(i);
				state.selectedPrefabPath = session.path;
				state.selectedPrefabEntity = session.prefabEntity;
				state.selectedEntity = nullptr;
				state.activeEntity = nullptr;
				state.selectedEntities.clear();
			}
			else
			{
				session.focused = false;
			}

			session.requestFocus = false;

			session.cameraSettings.viewportWidth = ImGui::GetContentRegionAvail().x;
			session.cameraSettings.viewportHeight = ImGui::GetContentRegionAvail().y;

			DrawPrefabSessionTexture(session);
		}
		ImGui::End();

		session.open = open;

		if (!session.open)
		{
			//DestroyPrefabPreviewFramebuffer(session);
			state.prefabPreviewSessions.erase(state.prefabPreviewSessions.begin() + i);

			if (state.activePrefabPreviewIndex == static_cast<int>(i))
				state.activePrefabPreviewIndex = -1;
			else if (state.activePrefabPreviewIndex > static_cast<int>(i))
				--state.activePrefabPreviewIndex;

			continue;
		}

		++i;
	}
}

void buki::InspectorPanel::RenderPrefabInspector(EditorState& state, IWorld& world)
{
	Entity* entity = state.selectedPrefabEntity;
	if (entity == nullptr)
	{
		ImGui::TextUnformatted("No prefab selected.");
		return;
	}

	bool changed = false;

	ImGui::Text("Prefab: %s", buki::GetFileName(state.selectedPrefabPath).c_str());
	ImGui::Separator();

	if (ImGui::Button("Save Prefab"))
	{
		if (world.SavePrefabAsset(state.selectedPrefabPath))
		{
			const std::string key = fs::absolute(state.selectedPrefabPath).lexically_normal().string();
			auto& prefabAssets = world.GetPrefabAssets();
			auto it = prefabAssets.find(key);
			if (it != prefabAssets.end())
				it->second.dirty = false;
		}
	}

	ImGui::Spacing();
	changed |= DrawPrefabEntitySection(entity);

	std::string componentToRemove;

	if (ImGui::CollapsingHeader("Components", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto& [type, component] : entity->GetComponents())
		{
			if (component == nullptr)
				continue;

			std::string cmpName = ComponentFactory::GetTypeName(*type);
			if (cmpName.empty())
				cmpName = type->name();

			bool componentChanged = false;

			componentChanged = InspectorPropertyDrawer::DrawComponent(component, &cmpName);

			changed |= componentChanged;

			if (ImGui::Button(("Remove##Prefab_" + cmpName).c_str()))
				componentToRemove = cmpName;
		}
	}

	if (!componentToRemove.empty())
		changed |= entity->RemoveComponentByTypeName(componentToRemove);

	changed |= InspectorPropertyDrawer::DrawAddComponentPopup(entity);

	if (changed)
	{
		entity->Set();

		const std::string key = fs::absolute(state.selectedPrefabPath).lexically_normal().string();
		auto& prefabAssets = world.GetPrefabAssets();
		auto it = prefabAssets.find(key);
		if (it != prefabAssets.end())
			it->second.dirty = true;
	}
}

bool buki::InspectorPanel::DrawPrefabEntitySection(Entity* entity)
{
	bool changed = false;

	if (ImGui::CollapsingHeader("Entity", ImGuiTreeNodeFlags_DefaultOpen))
	{
		char nameBuffer[256];
		std::snprintf(nameBuffer, sizeof(nameBuffer), "%s", entity->GetName().c_str());
		if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
		{
			entity->SetName(nameBuffer);
			changed = true;
		}

		bool enabled = entity->IsEnabled();
		if (ImGui::Checkbox("Enabled", &enabled))
		{
			entity->SetEnable(enabled);
			changed = true;
		}

		if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto t = entity->Tm();
			Vector2 position = t.GetPosition();
			float pos[2] = { position.x, position.y };
			if (ImGui::DragFloat2("Position", pos, 0.1f, 0.0f, 0.0f, "%.3f"))
			{
				t.SetPosition(Vector2{ pos[0], pos[1] });
				changed = true;
			}

			Vector2 size = t.GetSize();
			float sizeValues[2] = { size.x, size.y };
			if (ImGui::DragFloat2("Size", sizeValues, 0.1f, 0.0f, 0.0f, "%.3f"))
			{
				t.SetSize(Vector2{ sizeValues[0], sizeValues[1] });
				changed = true;
			}

			float rotation = t.GetRotation().GetRadians();
			if (ImGui::DragFloat("Rotation", &rotation, 0.1f, 0.0f, 0.0f, "%.3f"))
			{
				t.SetRotation(rotation);
				changed = true;
			}

			ImGui::TreePop();
		}

		int z = entity->GetZ();
		if (ImGui::InputInt("Z", &z))
		{
			entity->SetZ(z);
			changed = true;
		}

		std::string layer = entity->GetLayer();
		char layerBuffer[256];
		std::snprintf(layerBuffer, sizeof(layerBuffer), "%s", layer.c_str());
		if (ImGui::InputText("Layer", layerBuffer, sizeof(layerBuffer)))
		{
			entity->SetLayer(layerBuffer);
			changed = true;
		}
	}

	return changed;
}