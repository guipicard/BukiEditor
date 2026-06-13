#include "HierarchyPanel.h"

#include "Engine.h"
#include "Entity.h"
#include "imgui.h"

#include <string>
#include <vector>
#include <algorithm>

#include "nlohmann/json.hpp"
#include <fstream>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace
{
	bool ContainsEntity(const std::vector<buki::Entity*>& entities, buki::Entity* entity)
	{
		return std::find(entities.begin(), entities.end(), entity) != entities.end();
	}

	void RemoveEntity(std::vector<buki::Entity*>& entities, buki::Entity* entity)
	{
		entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
	}

	void SyncLegacySelection(buki::EditorState& state)
	{
		if (!state.selectedEntities.empty())
		{
			state.selectedEntity = state.activeEntity != nullptr ? state.activeEntity : state.selectedEntities.back();
		}
		else
		{
			state.selectedEntity = nullptr;
			state.activeEntity = nullptr;
		}
	}

	void ClearInvalidSelection(buki::EditorState& state, const std::vector<buki::Entity*>& allEntities)
	{
		state.selectedEntities.erase(
			std::remove_if(state.selectedEntities.begin(), state.selectedEntities.end(),
				[&](buki::Entity* entity)
				{
					return entity == nullptr || !ContainsEntity(allEntities, entity);
				}),
			state.selectedEntities.end());

		if (state.activeEntity != nullptr && !ContainsEntity(allEntities, state.activeEntity))
		{
			state.activeEntity = nullptr;
		}

		SyncLegacySelection(state);
	}

	void ApplySelectionRequests(
		buki::EditorState& state,
		const std::vector<buki::Entity*>& visibleEntities,
		ImGuiMultiSelectIO* msIO)
	{
		if (msIO == nullptr)
		{
			return;
		}

		for (const ImGuiSelectionRequest& request : msIO->Requests)
		{
			if (request.Type == ImGuiSelectionRequestType_SetAll)
			{
				if (request.Selected)
				{
					state.selectedEntities = visibleEntities;
					state.activeEntity = state.selectedEntities.empty() ? nullptr : state.selectedEntities.back();
				}
				else
				{
					state.selectedEntities.clear();
					state.activeEntity = nullptr;
				}
			}
			else if (request.Type == ImGuiSelectionRequestType_SetRange)
			{
				int first = static_cast<int>(request.RangeFirstItem);
				int last = static_cast<int>(request.RangeLastItem);

				if (first < 0 || last < 0 || first >= static_cast<int>(visibleEntities.size()) || last >= static_cast<int>(visibleEntities.size()))
				{
					continue;
				}

				if (first > last)
				{
					std::swap(first, last);
				}

				for (int i = first; i <= last; ++i)
				{
					buki::Entity* entity = visibleEntities[i];
					if (entity == nullptr)
					{
						continue;
					}

					if (request.Selected)
					{
						if (!ContainsEntity(state.selectedEntities, entity))
						{
							state.selectedEntities.push_back(entity);
						}
						state.activeEntity = entity;
					}
					else
					{
						RemoveEntity(state.selectedEntities, entity);
						if (state.activeEntity == entity)
						{
							state.activeEntity = state.selectedEntities.empty() ? nullptr : state.selectedEntities.back();
						}
					}
				}
			}
		}

		SyncLegacySelection(state);
	}

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

void buki::HierarchyPanel::Render(EditorState& state)
{
	ImGui::Begin("Hierarchy", &state.showHierarchy);
	auto& world = buki::Engine::Get().World();

	if (state.activePrefabPreviewIndex >= 0 &&
		state.activePrefabPreviewIndex < static_cast<int>(state.prefabPreviewSessions.size()) &&
		state.prefabPreviewSessions[state.activePrefabPreviewIndex].open)
	{
		PrefabPreviewSession& session = state.prefabPreviewSessions[state.activePrefabPreviewIndex];
		Entity* prefab = session.prefabEntity;

		ImGui::TextUnformatted("Prefab");

		if (prefab != nullptr)
		{
			const std::string fileLabel = fs::path(session.path).stem().string();
			const std::string entityLabel = prefab->GetName().empty() ? "Unnamed Entity" : prefab->GetName();

			bool selected = (state.selectedPrefabEntity == prefab);
			std::string label = fileLabel + "##prefab-hierarchy-entry";

			if (ImGui::Selectable(label.c_str(), selected))
			{
				state.selectedPrefabEntity = prefab;
				state.selectedPrefabPath = session.path;
				state.selectedEntity = nullptr;
				state.activeEntity = nullptr;
				state.selectedEntities.clear();
				state.activePrefabPreviewIndex = static_cast<int>(state.activePrefabPreviewIndex);
			}

			ImGui::TextDisabled("Entity: %s", entityLabel.c_str());
		}
		ImGui::End();
		return;
	}

	const std::vector<Entity*> allEntities = world.GetEntitiesInWorld();
	SanitizeSelection(state, allEntities);

	static char search[128] = {};
	ImGui::InputTextWithHint("##search", "Search entities...", search, sizeof(search));
	ImGui::SameLine();

	if (ImGui::Button("Create Entity"))
	{
		Entity* entity = world.CreateEntity("New Entity");
		if (entity != nullptr)
		{
			auto& t = entity->T();
			t.SetPosition({ 0.0f, 0.0f });
			t.SetSize({ 1.0f, 1.0f });
			t.SetRotation(0.0f);

			state.selectedPrefabPath.clear();
			state.selectedPrefabEntity = nullptr;
			state.selectedEntities.clear();
			state.selectedEntities.push_back(entity);
			state.activeEntity = entity;
			SyncLegacySelection(state);
			state.sceneDirty = true;
		}
	}

	std::vector<Entity*> toDelete;
	if (!state.selectedEntities.empty())
	{
		ImGui::SameLine();
		if (ImGui::Button("Delete Selected"))
		{

			for (Entity* entity : state.selectedEntities)
			{
				if (entity != nullptr)
				{
					toDelete.push_back(entity);
				}
			}
			state.selectedEntities.clear();
			state.activeEntity = nullptr;
			SyncLegacySelection(state);
			state.sceneDirty = true;
		}
	}

	ClearInvalidSelection(state, allEntities);

	std::vector<Entity*> visibleEntities;
	visibleEntities.reserve(allEntities.size());

	for (Entity* entity : allEntities)
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

		visibleEntities.push_back(entity);
	}

	ImGui::Text("Entities: %d", static_cast<int>(visibleEntities.size()));

	ImGuiMultiSelectFlags flags =
		ImGuiMultiSelectFlags_ClearOnClickVoid |
		ImGuiMultiSelectFlags_BoxSelect1d |
		ImGuiMultiSelectFlags_SelectOnClick |
		ImGuiMultiSelectFlags_ScopeWindow;

	ImGuiMultiSelectIO* msIO = ImGui::BeginMultiSelect(
		flags,
		static_cast<int>(state.selectedEntities.size()),
		static_cast<int>(visibleEntities.size()));

	ApplySelectionRequests(state, visibleEntities, msIO);

	for (int i = 0; i < static_cast<int>(visibleEntities.size()); ++i)
	{
		Entity* entity = visibleEntities[i];
		if (entity == nullptr)
		{
			continue;
		}

		std::string displayName = entity->GetName();
		if (displayName.empty())
		{
			displayName = "Unnamed Entity";
		}

		const bool selected = ContainsEntity(state.selectedEntities, entity);

		ImGui::SetNextItemSelectionUserData(static_cast<ImGuiSelectionUserData>(i));
		ImGui::Selectable(displayName.c_str(), selected);

		if (ImGui::IsItemClicked())
		{
			state.selectedPrefabPath.clear();
			state.selectedPrefabEntity = nullptr;
			state.activeEntity = entity;
			SyncLegacySelection(state);
		}

		if (ImGui::BeginDragDropSource())
		{
			Entity* draggedEntity = entity;
			ImGui::SetDragDropPayload("ENTITY", &draggedEntity, sizeof(Entity*));
			ImGui::TextUnformatted(entity->GetName().c_str());
			ImGui::EndDragDropSource();
		}
	}

	msIO = ImGui::EndMultiSelect();
	ApplySelectionRequests(state, visibleEntities, msIO);

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

	for (Entity* entity : toDelete)
	{
		if (entity != nullptr)
		{
			world.RemoveFromScene(entity);
		}
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