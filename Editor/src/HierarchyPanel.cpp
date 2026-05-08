#include "HierarchyPanel.h"

#include "Engine.h"
#include "Entity.h"
#include "imgui.h"

#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include <fstream>

namespace fs = std::filesystem;
using json = nlohmann::json;

void buki::HierarchyPanel::Render(EditorState& state)
{
	ImGui::Begin("Hierarchy", &state.showHierarchy);
	auto& world = buki::Engine::Get().World();



	static char search[128] = {};
	ImGui::InputTextWithHint("##search", "Search entities...", search, sizeof(search));
	ImGui::SameLine();

	if (ImGui::Button("Create Entity"))
	{
		Entity* entity = world.CreateEntity("New Entity");
		if (entity != nullptr)
		{
			if (entity->T() != nullptr)
			{
				entity->T()->SetPosition({ 0.0f, 0.0f });
				entity->T()->SetSize({ 1.0f, 1.0f });
				entity->T()->SetRotation(0.0f);
			}

			state.selectedEntity = entity;
			state.sceneDirty = true;
		}
	}

	if (state.selectedEntity != nullptr)
	{
		ImGui::SameLine();
		if (ImGui::Button("Delete Selected"))
		{
			world.RemoveFromScene(state.selectedEntity);
			state.selectedEntity = nullptr;
			state.sceneDirty = true;
		}
	}


	const std::vector<Entity*> entities = world.GetEntitiesInWorld();

	ImGui::Text("Entities: %d", entities.size());

	for (Entity* entity : entities)
	{
		if (entity == nullptr)
		{
			continue;
		}

		std::string displayName = entity->GetName();
		if (displayName.empty())
		{
			displayName = "Unnamed Entity";
		}
		if (search[0] != '\0' && displayName.find(search) == std::string::npos)
		{
			continue;
		}

		const bool selected = (state.selectedEntity == entity);
		if (ImGui::Selectable(displayName.c_str(), selected))
		{
			state.selectedEntity = entity;
		}
		if (ImGui::BeginDragDropSource())
		{
			Entity* draggedEntity = entity;
			ImGui::SetDragDropPayload("ENTITY", &draggedEntity, sizeof(Entity*));
			ImGui::TextUnformatted(entity->GetName().c_str());
			ImGui::EndDragDropSource();
		}
	}


	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PREFAB"))
		{
			const char* droppedPath = static_cast<const char*>(payload->Data);
			if (droppedPath != nullptr)
			{
				world.InstantiatePrefab(fs::path(droppedPath).string());
			}
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::End();
}

void buki::HierarchyPanel::SaveEntityAsPrefab(Entity* entity, const std::filesystem::path& path)
{
	if (!entity) return;
	json prefabData = entity->Serialize();
	std::ofstream out(path);
	if (out.is_open())
	{
		out << prefabData.dump(4);
	}
}