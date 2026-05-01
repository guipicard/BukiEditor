#include "HierarchyPanel.h"

#include "Engine.h"
#include "Entity.h"
#include "imgui.h"

#include <string>
#include <vector>

void buki::HierarchyPanel::Render(EditorState& state)
{
	ImGui::Begin("Hierarchy", &state.showHierarchy);

	auto* world = buki::Engine::Get().GetWorldPtr();
	if (world == nullptr)
	{
		ImGui::TextUnformatted("World not initialized.");
		ImGui::End();
		return;
	}

	static char search[128] = {};
	ImGui::InputTextWithHint("##search", "Search entities...", search, sizeof(search));
	ImGui::SameLine();

	if (ImGui::Button("Create Entity"))
	{
		Entity* entity = world->CreateEntity("New Entity");
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
			state.selectedEntity->Destroy();
			state.selectedEntity = nullptr;
			state.sceneDirty = true;
		}
	}

	ImGui::Separator();

	const std::vector<Entity*> entities = world->GetEntitiesInWorld();
	ImGui::Text("Entities: %d", static_cast<int>(entities.size()));
	ImGui::Separator();

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
	}

	ImGui::End();
}