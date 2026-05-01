#include "InspectorPanel.h"

#include "Entity.h"
#include "BukiContainers.h"
#include "imgui.h"

#include <array>
#include <cstring>
#include <string>

bool buki::InspectorPanel::DrawStringField(const char* label, std::string& value)
{
	std::array<char, 256> buffer{};
	std::strncpy(buffer.data(), value.c_str(), buffer.size() - 1);
	if (ImGui::InputText(label, buffer.data(), buffer.size()))
	{
		value = buffer.data();
		return true;
	}
	return false;
}

void buki::InspectorPanel::Render(EditorState& state)
{
	ImGui::Begin("Inspector", &state.showInspector);

	if (state.selectedEntity == nullptr)
	{
		ImGui::TextUnformatted("No entity selected.");
		ImGui::End();
		return;
	}

	Entity* entity = state.selectedEntity;
	ImGui::TextUnformatted("Entity");
	ImGui::Separator();

	std::string entityName = entity->GetName();
	if (DrawStringField("Name", entityName))
	{
		entity->SetName(entityName);
		state.sceneDirty = true;
	}

	int z = entity->GetZ();
	if (ImGui::DragInt("Z", &z, 1.0f))
	{
		entity->SetZ(z);
		state.sceneDirty = true;
	}

	std::string layer = entity->GetLayer();
	if (DrawStringField("Layer", layer))
	{
		entity->SetLayer(layer);
		state.sceneDirty = true;
	}

	bool enabled = entity->GetEnabled();
	if (ImGui::Checkbox("Enabled", &enabled))
	{
		if (enabled) entity->Enable(); else entity->Disable();
		state.sceneDirty = true;
	}

	Transform* transform = entity->T();
	if (transform != nullptr)
	{
		ImGui::Spacing();
		ImGui::TextUnformatted("Transform");
		ImGui::Separator();

		Vector2 pos = transform->GetPosition();
		float p[2] = { pos.x, pos.y };
		if (ImGui::DragFloat2("Position", p, 0.01f))
		{
			transform->SetPosition({ p[0], p[1] });
			state.sceneDirty = true;
		}

		float rot = transform->GetRotation().GetRadians();
		if (ImGui::DragFloat("Rotation", &rot, 0.01f))
		{
			transform->SetRotation(rot);
			state.sceneDirty = true;
		}

		Vector2 size = transform->GetSize();
		float s[2] = { size.x, size.y };
		if (ImGui::DragFloat2("Size", s, 0.01f, 0.0f, 10000.0f))
		{
			transform->SetSize({ s[0], s[1] });
			state.sceneDirty = true;
		}
	}

	ImGui::End();
}
