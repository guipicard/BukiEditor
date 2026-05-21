#pragma once 
#include "InspectorPanel.h"

#include "Entity.h"
#include "BukiContainers.h"
#include "imgui.h"
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

#include <cstdlib>
#include <cstring>
#include <unordered_map>

#include <fstream>
#include <algorithm>

namespace fs = std::filesystem;

namespace
{

	float ClampMinFloat(float value, float minValue)
	{
		return (value < minValue) ? minValue : value;
	}

	float ClampFloat(float value, float minValue, float maxValue)
	{
		if (value < minValue) return minValue;
		if (value > maxValue) return maxValue;
		return value;
	}

	int ClampMinInt(int value, int minValue)
	{
		return (value < minValue) ? minValue : value;
	}

	buki::Vector2 ClampMinVector2(const buki::Vector2& value, float minValue)
	{
		buki::Vector2 result = value;
		if (result.x < minValue) result.x = minValue;
		if (result.y < minValue) result.y = minValue;
		return result;
	}

	template<typename T>
	T* GetSharedComponentAs(const std::vector<buki::Entity*>& entities, const std::string& typeName)
	{
		if (entities.empty() || entities[0] == nullptr)
			return nullptr;

		buki::Component* component = entities[0]->GetComponentByTypeName(typeName);
		if (component == nullptr)
			return nullptr;

		return dynamic_cast<T*>(component);
	}

	template<typename T>
	std::vector<T*> GatherSharedComponents(const std::vector<buki::Entity*>& entities, const std::string& typeName)
	{
		std::vector<T*> result;
		result.reserve(entities.size());

		for (buki::Entity* entity : entities)
		{
			if (entity == nullptr)
				return {};

			buki::Component* component = entity->GetComponentByTypeName(typeName);
			if (component == nullptr)
				return {};

			T* typed = dynamic_cast<T*>(component);
			if (typed == nullptr)
				return {};

			result.push_back(typed);
		}

		return result;
	}

	bool AreAllEqualBool(const std::vector<bool>& values)
	{
		if (values.empty())
			return true;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (values[i] != values[0])
				return false;
		}
		return true;
	}

	bool AreAllEqualInt(const std::vector<int>& values)
	{
		if (values.empty())
			return true;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (values[i] != values[0])
				return false;
		}
		return true;
	}

	bool AreAllEqualFloat(const std::vector<float>& values)
	{
		if (values.empty())
			return true;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (values[i] != values[0])
				return false;
		}
		return true;
	}

	bool AreAllEqualString(const std::vector<std::string>& values)
	{
		if (values.empty())
			return true;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (values[i] != values[0])
				return false;
		}
		return true;
	}

	bool AreAllEqualColor(const std::vector<buki::Color>& values)
	{
		if (values.empty())
			return true;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (values[i].r != values[0].r ||
				values[i].g != values[0].g ||
				values[i].b != values[0].b ||
				values[i].a != values[0].a)
			{
				return false;
			}
		}
		return true;
	}

	bool AreAllEqualRectF(const std::vector<buki::RectF>& values)
	{
		if (values.empty())
			return true;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (values[i].x != values[0].x ||
				values[i].y != values[0].y ||
				values[i].w != values[0].w ||
				values[i].h != values[0].h)
			{
				return false;
			}
		}
		return true;
	}

	void DrawMixedLabel(const char* label)
	{
		ImGui::TextUnformatted(label);
		ImGui::SameLine();
		ImGui::TextDisabled("(mixed)");
	}

	std::string ToLowerCopy(std::string value)
	{
		std::transform(value.begin(), value.end(), value.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		return value;
	}

	bool IsImageAssetPath(const fs::path& path)
	{
		std::string ext = ToLowerCopy(path.extension().string());
		return ext == ".png" ||
			ext == ".jpg" ||
			ext == ".jpeg" ||
			ext == ".bmp" ||
			ext == ".tga" ||
			ext == ".gif" ||
			ext == ".webp";
	}

	std::string ToAssetRelativePath(const fs::path& fullPath)
	{
		std::string normalized = fullPath.lexically_normal().generic_string();
		const std::string marker = "/assets/";
		size_t pos = normalized.find(marker);
		if (pos != std::string::npos)
			return normalized.substr(pos);
		return normalized;
	}

	std::string DisplayNameForAssetPath(const std::string& assetPath)
	{
		if (assetPath.empty())
			return "";

		return fs::path(assetPath).filename().string();
	}

	std::string FolderSuffixForAssetPath(const std::string& assetPath)
	{
		fs::path p(assetPath);
		fs::path parent = p.parent_path();
		if (parent.empty())
			return "";

		return "[" + parent.generic_string() + "]";
	}

	std::vector<std::string> CollectImageAssets(const fs::path& root)
	{
		std::vector<std::string> results;

		if (!fs::exists(root) || !fs::is_directory(root))
			return results;

		for (const auto& entry : fs::recursive_directory_iterator(root))
		{
			if (!entry.is_regular_file())
				continue;

			if (!IsImageAssetPath(entry.path()))
				continue;

			results.push_back(ToAssetRelativePath(entry.path()));
		}

		std::sort(results.begin(), results.end());
		return results;
	}

	struct MixedTextState
	{
		bool initialized = false;
		std::string value;
	};

	static std::unordered_map<ImGuiID, MixedTextState> s_MixedTextStates;

	MixedTextState& GetMixedTextState(ImGuiID id, const std::string& initialValue)
	{
		MixedTextState& state = s_MixedTextStates[id];
		if (!state.initialized)
		{
			state.initialized = true;
			state.value = initialValue;
		}
		return state;
	}

	void ResetMixedTextState(ImGuiID id)
	{
		s_MixedTextStates.erase(id);
	}

	bool AreAllEqualVector2X(const std::vector<buki::Vector2>& values)
	{
		if (values.empty())
			return true;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (values[i].x != values[0].x)
				return false;
		}
		return true;
	}

	bool AreAllEqualVector2Y(const std::vector<buki::Vector2>& values)
	{
		if (values.empty())
			return true;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (values[i].y != values[0].y)
				return false;
		}
		return true;
	}

	template<typename T>
	T& Access(void* instance, const buki::PropertyInfo& info)
	{
		return *reinterpret_cast<T*>(reinterpret_cast<std::uint8_t*>(instance) + info.offset);
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

void buki::InspectorPanel::Render(EditorState& state)
{
	ImGui::Begin("Inspector", &state.showInspector);

	auto& world = Engine::Get().World();
	const std::vector<Entity*> allEntities = world.GetEntitiesInWorld();

	SanitizeSelection(state, allEntities);

	std::vector<Entity*> selected;
	selected.reserve(state.selectedEntities.size());


	for (Entity* entity : state.selectedEntities)
	{
		if (entity != nullptr && ContainsEntityPtr(allEntities, entity))
		{
			selected.push_back(entity);
		}
	}

	if (selected.empty() && state.selectedEntity != nullptr && ContainsEntityPtr(allEntities, state.selectedEntity))
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
		Entity* entity = selected[0];
		state.selectedEntity = entity;
		state.activeEntity = entity;

		bool changed = false;
		ImGui::Text("Entity: %s", entity->GetName().c_str());
		ImGui::Separator();

		if (ImGui::Button("Save Prefab"))
		{
			changed |= SaveSelectedEntityAsPrefab(entity);
		}

		ImGui::Spacing();
		changed |= DrawEntitySection(entity);

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

				if (dynamic_cast<Text*>(component))
					componentChanged = DrawTextComponent(component);
				else if (dynamic_cast<Button*>(component))
					componentChanged = DrawButtonComponent(component);
				else if (dynamic_cast<Sprite*>(component))
					componentChanged = DrawSpriteComponent(component);
				else if (dynamic_cast<TileLayer*>(component))
					componentChanged = DrawTileLayerComponent(component, state);
				else if (dynamic_cast<RigidBody*>(component))
					componentChanged = DrawRigidBodyComponent(component);
				else if (dynamic_cast<Box*>(component))
					componentChanged = DrawBoxComponent(component);
				else if (dynamic_cast<Circle*>(component))
					componentChanged = DrawCircleComponent(component);
				else if (dynamic_cast<Polygon*>(component))
					componentChanged = DrawPolygonComponent(component);
				else if (dynamic_cast<MonoBehaviour*>(component))
					componentChanged = InspectorPropertyDrawer::DrawComponent(component, &cmpName);
				else
					ImGui::BulletText("%s", type->name());

				changed |= componentChanged;

				if (ImGui::Button(("Remove " + cmpName).c_str()))
				{
					componentToRemove = cmpName;
				}
			}

			if (!componentToRemove.empty())
			{
				changed |= entity->RemoveComponentByTypeName(componentToRemove);
			}

			ImGui::Separator();
			changed |= InspectorPropertyDrawer::DrawAddComponentPopup(entity);
		}

		if (changed)
		{
			state.sceneDirty = true;
		}

		ImGui::End();
		return;
	}

	state.selectedEntity = state.activeEntity != nullptr ? state.activeEntity : selected.front();

	bool changed = false;
	ImGui::Text("Entities Selected: %d", static_cast<int>(selected.size()));
	ImGui::Separator();

	changed |= DrawMultiEntitySection(selected);

	if (ImGui::CollapsingHeader("Shared Components", ImGuiTreeNodeFlags_DefaultOpen))
	{
		changed |= InspectorPropertyDrawer::DrawAddComponentPopup(selected);

		std::vector<std::string> sharedComponentTypes = InspectorPropertyDrawer::GetSharedComponentTypeNames(selected);
		std::string componentToRemove;

		for (const std::string& typeName : sharedComponentTypes)
		{
			if (typeName == "Transform")
			{
				continue;
			}

			if (typeName == "Text")
			{
				if (ImGui::TreeNodeEx("Text", ImGuiTreeNodeFlags_DefaultOpen))
				{
					changed |= DrawSharedTextComponents(selected);
					ImGui::SameLine();
					if (ImGui::Button("Remove##Text"))
						componentToRemove = "Text";
					ImGui::TreePop();
				}
			}
			else if (typeName == "Button")
			{
				if (ImGui::TreeNodeEx("Button", ImGuiTreeNodeFlags_DefaultOpen))
				{
					changed |= DrawSharedButtonComponents(selected);
					ImGui::SameLine();
					if (ImGui::Button("Remove##Button"))
						componentToRemove = "Button";
					ImGui::TreePop();
				}
			}
			else if (typeName == "Sprite")
			{
				if (ImGui::TreeNodeEx("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
				{
					changed |= DrawSharedSpriteComponents(selected);
					ImGui::SameLine();
					if (ImGui::Button("Remove##Sprite"))
						componentToRemove = "Sprite";
					ImGui::TreePop();
				}
			}
			else if (typeName == "RigidBody")
			{
				if (ImGui::TreeNodeEx("RigidBody", ImGuiTreeNodeFlags_DefaultOpen))
				{
					changed |= DrawSharedRigidBodyComponents(selected);
					ImGui::SameLine();
					if (ImGui::Button("Remove##RigidBody"))
						componentToRemove = "RigidBody";
					ImGui::TreePop();
				}
			}
			else if (typeName == "Box")
			{
				if (ImGui::TreeNodeEx("Box", ImGuiTreeNodeFlags_DefaultOpen))
				{
					changed |= DrawSharedBoxComponents(selected);
					ImGui::SameLine();
					if (ImGui::Button("Remove##Box"))
						componentToRemove = "Box";
					ImGui::TreePop();
				}
			}
			else if (typeName == "Circle")
			{
				if (ImGui::TreeNodeEx("Circle", ImGuiTreeNodeFlags_DefaultOpen))
				{
					changed |= DrawSharedCircleComponents(selected);
					ImGui::SameLine();
					if (ImGui::Button("Remove##Circle"))
						componentToRemove = "Circle";
					ImGui::TreePop();
				}
			}
			else if (typeName == "Polygon")
			{
				if (ImGui::TreeNodeEx("Polygon", ImGuiTreeNodeFlags_DefaultOpen))
				{
					changed |= DrawSharedPolygonComponents(selected);
					ImGui::SameLine();
					if (ImGui::Button("Remove##Polygon"))
						componentToRemove = "Polygon";
					ImGui::TreePop();
				}
			}
			else
			{
				if (ImGui::TreeNodeEx(typeName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					changed |= InspectorPropertyDrawer::DrawSharedComponent(typeName, selected);
					ImGui::SameLine();
					if (ImGui::Button(("Remove##" + typeName).c_str()))
					{
						componentToRemove = typeName;
					}
					ImGui::TreePop();
				}
			}
		}

		if (!componentToRemove.empty())
		{
			for (Entity* entity : selected)
			{
				if (entity != nullptr)
				{
					changed |= entity->RemoveComponentByTypeName(componentToRemove);
				}
			}
		}
	}

	if (changed)
	{
		state.sceneDirty = true;
	}

	ImGui::End();
}

bool buki::InspectorPanel::DrawMultiEntitySection(const std::vector<Entity*>& entities)
{
	if (entities.empty())
	{
		return false;
	}

	bool changed = false;

	if (ImGui::CollapsingHeader("Entity", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool allEnabled = true;
		bool anyEnabled = false;

		for (Entity* entity : entities)
		{
			if (entity == nullptr)
			{
				continue;
			}

			if (entity->IsEnabled())
				anyEnabled = true;
			else
				allEnabled = false;
		}

		bool enabledValue = allEnabled;
		const bool mixedEnabled = (allEnabled != anyEnabled);
		if (DrawMixedBoolField("Enabled", mixedEnabled, enabledValue))
		{
			for (Entity* entity : entities)
			{
				if (entity != nullptr)
					entity->SetEnable(enabledValue);
			}
			changed = true;
		}

		changed |= DrawMultiTransformSection(entities);

		int firstZ = entities.front()->GetZ();
		bool mixedZ = false;
		for (Entity* entity : entities)
		{
			if (entity != nullptr && entity->GetZ() != firstZ)
			{
				mixedZ = true;
				break;
			}
		}

		int z = firstZ;
		if (DrawMixedIntField("Z", mixedZ, z))
		{
			for (Entity* entity : entities)
			{
				if (entity != nullptr)
					entity->SetZ(z);
			}
			Engine::Get().World().SortEntities();
			changed = true;
		}
	}

	return changed;
}

bool buki::InspectorPanel::DrawMultiTransformSection(const std::vector<Entity*>& entities)
{
	if (entities.empty())
	{
		return false;
	}

	bool changed = false;

	if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Vector2 firstPosition = entities.front()->T()->GetPosition();
		Vector2 firstSize = entities.front()->T()->GetSize();
		float firstRotation = entities.front()->T()->GetRotation().GetRadians();

		std::vector<Vector2> positions;
		std::vector<Vector2> sizes;
		positions.reserve(entities.size());
		sizes.reserve(entities.size());

		bool mixedRotation = false;

		for (Entity* entity : entities)
		{
			if (entity == nullptr || entity->T() == nullptr)
			{
				continue;
			}

			Vector2 position = entity->T()->GetPosition();
			Vector2 size = entity->T()->GetSize();
			float rotation = entity->T()->GetRotation().GetRadians();

			positions.push_back(position);
			sizes.push_back(size);

			if (rotation != firstRotation)
				mixedRotation = true;
		}

		const bool mixedPosX = !AreAllEqualVector2X(positions);
		const bool mixedPosY = !AreAllEqualVector2Y(positions);

		float posX = firstPosition.x;
		float posY = firstPosition.y;
		float deltaPosX = 0.0f;
		float deltaPosY = 0.0f;
		bool changedPosX = false;
		bool changedPosY = false;

		if (DrawMixedVector2DragDeltaFieldPerAxis(
			"Position",
			mixedPosX,
			mixedPosY,
			posX,
			posY,
			deltaPosX,
			deltaPosY,
			changedPosX,
			changedPosY,
			"%.3f"))
		{
			for (Entity* entity : entities)
			{
				if (entity == nullptr || entity->T() == nullptr)
					continue;

				Vector2 value = entity->T()->GetPosition();

				if (changedPosX)
				{
					if (mixedPosX)
						value.x += deltaPosX;
					else
						value.x = posX;
				}

				if (changedPosY)
				{
					if (mixedPosY)
						value.y += deltaPosY;
					else
						value.y = posY;
				}

				entity->T()->SetPosition(value);
			}
			changed = true;
		}

		float sizeX = firstSize.x;
		float sizeY = firstSize.y;
		bool changedSizeX = false;
		bool changedSizeY = false;
		if (DrawMixedVector2FieldPerAxis(
			"Size",
			!AreAllEqualVector2X(sizes),
			!AreAllEqualVector2Y(sizes),
			sizeX,
			sizeY,
			changedSizeX,
			changedSizeY,
			"%.3f"))
		{
			for (Entity* entity : entities)
			{
				if (entity != nullptr && entity->T() != nullptr)
				{
					Vector2 value = entity->T()->GetSize();
					if (changedSizeX) value.x = sizeX;
					if (changedSizeY) value.y = sizeY;
					entity->T()->SetSize(value);
				}
			}
			changed = true;
		}

		float rotation = firstRotation;
		float previousRotation = rotation;

		if (mixedRotation)
		{
			DrawMixedLabel("Rotation");
			if (ImGui::DragFloat("##Rotation", &rotation, 0.1f, 0.0f, 0.0f, "%.3f"))
			{
				float deltaRotation = rotation - previousRotation;

				for (Entity* entity : entities)
				{
					if (entity != nullptr && entity->T() != nullptr)
					{
						float currentRotation = entity->T()->GetRotation().GetRadians();
						entity->T()->SetRotation(currentRotation + deltaRotation);
					}
				}

				changed = true;
			}
		}
		else
		{
			if (ImGui::DragFloat("Rotation", &rotation, 0.1f, 0.0f, 0.0f, "%.3f"))
			{
				for (Entity* entity : entities)
				{
					if (entity != nullptr && entity->T() != nullptr)
					{
						entity->T()->SetRotation(rotation);
					}
				}

				changed = true;
			}
		}

		ImGui::TreePop();
	}

	return changed;
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

bool buki::InspectorPanel::DrawEntitySection(Entity* entity)
{
	bool changed = false;

	if (ImGui::CollapsingHeader("Entity", ImGuiTreeNodeFlags_DefaultOpen))
	{
		char nameBuffer[256] = {};
		std::snprintf(nameBuffer, sizeof(nameBuffer), "%s", entity->GetName().c_str());

		if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
		{
			if (buki::Engine::Get().World().RenameEntity(entity, nameBuffer))
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
			Vector2 position = entity->T()->GetPosition();
			float pos[2] = { position.x, position.y };
			if (ImGui::DragFloat2("Position", pos, 0.1f, 0.0f, 0.0f, "%.3f"))
			{
				entity->T()->SetPosition(Vector2(pos[0], pos[1]));
				changed = true;
			}

			Vector2 size = entity->T()->GetSize();
			float sizeValues[2] = { size.x, size.y };
			if (ImGui::DragFloat2("Size", sizeValues, 0.1f, 0.0f, 0.0f, "%.3f"))
			{
				entity->T()->SetSize(Vector2(sizeValues[0], sizeValues[1]));
				changed = true;
			}

			float rotation = entity->T()->GetRotation().GetRadians();
			if (ImGui::DragFloat("Rotation", &rotation, 0.1f, 0.0f, 0.0f, "%.3f"))
			{
				entity->T()->SetRotation(rotation);
				changed = true;
			}

			ImGui::TreePop();
		}

		int z = entity->GetZ();
		if (ImGui::InputInt("Z", &z))
		{
			entity->SetZ(z);
			Engine::Get().World().SortEntities();
			changed = true;
		}

		std::string layer = entity->GetLayer();
		char layerBuffer[256] = {};
		std::snprintf(layerBuffer, sizeof(layerBuffer), "%s", layer.c_str());
		if (ImGui::InputText("Layer", layerBuffer, sizeof(layerBuffer)))
		{
			entity->SetLayer(layerBuffer);
			changed = true;
		}
	}

	return changed;
}

bool buki::InspectorPanel::DrawTextComponent(Component* cmp)
{
	Text* text = dynamic_cast<Text*>(cmp);
	if (text == nullptr) return false;
	bool changed = false;
	if (ImGui::TreeNodeEx("Text", ImGuiTreeNodeFlags_DefaultOpen))
	{
		char textBuffer[512] = {};
		std::snprintf(textBuffer, sizeof(textBuffer), "%s", text->GetText().c_str());
		if (ImGui::InputTextMultiline("Content", textBuffer, sizeof(textBuffer))) { text->SetText(textBuffer); changed = true; }

		char fontPathBuffer[512] = {};
		std::snprintf(fontPathBuffer, sizeof(fontPathBuffer), "%s", text->GetFontPath().c_str());
		if (ImGui::InputText("Font Path", fontPathBuffer, sizeof(fontPathBuffer))) { text->SetFontPath(fontPathBuffer); changed = true; }

		if (ImGui::InputInt("Font Size", text->GetFontSizeRef())) changed = true;
		if (ImGui::Checkbox("Center X", text->GetCenterXRef())) changed = true;
		if (ImGui::Checkbox("Center Y", text->GetCenterYRef())) changed = true;

		Color color = text->GetColor();
		float colorValues[4] = { color.r, color.g, color.b, color.a };
		if (ImGui::ColorEdit4("Color", colorValues)) { text->SetColor({ colorValues[0], colorValues[1], colorValues[2], colorValues[3] }); changed = true; }

		float offset[2] = { text->GetPositionOffset().x, text->GetPositionOffset().y };
		if (ImGui::InputFloat2("Offset", offset, "%.3f")) { text->SetPositionOffset(Vector2(offset[0], offset[1])); changed = true; }

		ImGui::TreePop();
	}
	return changed;
}

bool buki::InspectorPanel::DrawButtonComponent(Component* cmp)
{
	Button* button = dynamic_cast<Button*>(cmp);
	if (button == nullptr) return false;
	bool changed = false;
	if (ImGui::TreeNodeEx("Button", ImGuiTreeNodeFlags_DefaultOpen))
	{
		char textBuffer[256] = {};
		std::snprintf(textBuffer, sizeof(textBuffer), "%s", button->GetText().c_str());
		if (ImGui::InputText("Text", textBuffer, sizeof(textBuffer))) { button->SetText(textBuffer); changed = true; }

		char messageBuffer[256] = {};
		std::snprintf(messageBuffer, sizeof(messageBuffer), "%s", button->GetMessage().c_str());
		if (ImGui::InputText("Message", messageBuffer, sizeof(messageBuffer))) { button->SetMessage(messageBuffer); changed = true; }

		char fontPathBuffer[512] = {};
		std::snprintf(fontPathBuffer, sizeof(fontPathBuffer), "%s", button->GetFontPath().c_str());
		if (ImGui::InputText("Font Path", fontPathBuffer, sizeof(fontPathBuffer))) { button->SetFontPath(fontPathBuffer); changed = true; }

		int fontSize = button->GetFontSize();
		if (ImGui::DragInt("Font Size", &fontSize, 1.0f, 1, 60)) { button->SetFontSize(fontSize); changed = true; }

		bool centerX = button->Style().centerTextX;
		if (ImGui::Checkbox("Center Text X", &centerX)) { button->Style().centerTextX = centerX; changed = true; }

		bool centerY = button->Style().centerTextY;
		if (ImGui::Checkbox("Center Text Y", &centerY)) { button->Style().centerTextY = centerY; changed = true; }

		if (ImGui::Checkbox("Fit To Text", &button->Style().fitToText)) changed = true;
		if (ImGui::Checkbox("Show Background", &button->Style().showBackground)) changed = true;

		float bg[4] = { button->Style().backgroundColor.r, button->Style().backgroundColor.g, button->Style().backgroundColor.b, button->Style().backgroundColor.a };
		if (ImGui::ColorEdit4("Background", bg)) { button->Style().backgroundColor = { bg[0], bg[1], bg[2], bg[3] }; changed = true; }

		float textColor[4] = { button->Style().textColor.r, button->Style().textColor.g, button->Style().textColor.b, button->Style().textColor.a };
		if (ImGui::ColorEdit4("Text Color", textColor)) { button->Style().textColor = { textColor[0], textColor[1], textColor[2], textColor[3] }; changed = true; }

		ImGui::TreePop();
	}
	if (changed) { button->Set(); Engine::Get().Log().LogMessage("Button component updated."); }
	return changed;
}

bool buki::InspectorPanel::DrawSpriteComponent(Component* cmp)
{
	Sprite* sprite = dynamic_cast<Sprite*>(cmp);
	if (sprite == nullptr) return false;
	bool changed = false;
	if (ImGui::TreeNodeEx("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
	{
		char pathBuffer[512] = {};
		std::snprintf(pathBuffer, sizeof(pathBuffer), "%s", sprite->GetPath().c_str());
		std::string spritePath = sprite->GetPath();
		if (InspectorPropertyDrawer::DrawSpriteTexturePathField("Texture Path", spritePath))
		{
			sprite->SetPath(spritePath);
			buki::Engine::Get().Log().LogMessage("Asset selected: " + spritePath);
			changed = true;
		}

		Color color = sprite->GetColor();
		float colorValues[4] = { color.r, color.g, color.b, color.a };
		if (ImGui::ColorEdit4("Color", colorValues)) { sprite->SetColor({ colorValues[0], colorValues[1], colorValues[2], colorValues[3] }); changed = true; }

		Vector2 posOffset = sprite->GetPositionOffset();
		float posOffsetValues[2] = { posOffset.x, posOffset.y };
		if (ImGui::InputFloat2("Position Offset", posOffsetValues, "%.3f")) { sprite->SetPositionOffset({ posOffsetValues[0], posOffsetValues[1] }); changed = true; }

		Vector2 sizeOffset = sprite->GetSizeOffset();
		float sizeOffsetValues[2] = { sizeOffset.x, sizeOffset.y };
		if (ImGui::InputFloat2("Size Offset", sizeOffsetValues, "%.3f")) { sprite->SetSizeOffset({ sizeOffsetValues[0], sizeOffsetValues[1] }); changed = true; }

		bool flipX = sprite->GetFlipX();
		if (ImGui::Checkbox("Flip X", &flipX)) { sprite->SetFlipX(flipX); changed = true; }

		bool flipY = sprite->GetFlipY();
		if (ImGui::Checkbox("Flip Y", &flipY)) { sprite->SetFlipY(flipY); changed = true; }

		bool useSourceRect = sprite->UsesSourceRect();
		if (ImGui::Checkbox("Use Source Rect", &useSourceRect)) { sprite->SetUseSourceRect(useSourceRect); changed = true; }

		RectF src = sprite->GetSourceRectPixels();
		float srcValues[4] = { src.x, src.y, src.w, src.h };
		if (ImGui::InputFloat4("Source Rect", srcValues, "%.3f")) { sprite->SetSourceRectPixels({ srcValues[0], srcValues[1], srcValues[2], srcValues[3] }); changed = true; }

		ImGui::TreePop();

		if (changed) 
		{ 
			sprite->Set();
		}
	}
	return changed;
}

bool buki::InspectorPanel::DrawTileLayerComponent(Component* cmp, EditorState& state)
{
	TileLayer* tileLayer = dynamic_cast<TileLayer*>(cmp);
	if (tileLayer == nullptr) return false;
	bool changed = false;
	bool needRebuild = false;
	if (ImGui::TreeNodeEx("TileLayer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Apply To All Tiles", &state.setTilesDefaults);

		char atlasPathBuffer[512] = {};
		std::snprintf(atlasPathBuffer, sizeof(atlasPathBuffer), "%s", tileLayer->GetAtlasPath().c_str());
		if (ImGui::InputText("Atlas Path", atlasPathBuffer, sizeof(atlasPathBuffer))) { tileLayer->SetAtlasPath(atlasPathBuffer); changed = true; }

		Vector2 layerOffset = tileLayer->GetLayerOffset();
		float layerOffsetValues[2] = { layerOffset.x, layerOffset.y };
		if (ImGui::DragFloat2("Layer Offset", layerOffsetValues, 0.1f, 0.0f, 0.0f, "%.3f")) { tileLayer->SetLayerOffset({ layerOffsetValues[0], layerOffsetValues[1] }); changed = true; }

		Vector2 defaultTileSize = tileLayer->GetDefaultTileSize();
		float defaultTileSizeValues[2] = { defaultTileSize.x, defaultTileSize.y };
		if (ImGui::DragFloat2("Default Tile Size", defaultTileSizeValues, 0.1f, 0.0f, 0.0f, "%.3f")) { tileLayer->SetDefaultTileSize({ defaultTileSizeValues[0], defaultTileSizeValues[1] }); if (state.setTilesDefaults) needRebuild = true; changed = true; }

		RectF defaultSrc = tileLayer->GetDefaultSourceRectPixels();
		int defaultSrcValues[4] = { (int)defaultSrc.x, (int)defaultSrc.y, (int)defaultSrc.w, (int)defaultSrc.h };
		if (ImGui::DragInt4("Default Source Rect", defaultSrcValues, 1, 0, 0)) { tileLayer->SetDefaultSourceRectPixels({ (float)defaultSrcValues[0], (float)defaultSrcValues[1], (float)defaultSrcValues[2], (float)defaultSrcValues[3] }); if (state.setTilesDefaults) needRebuild = true; changed = true; }

		Color tint = tileLayer->GetTint();
		float tintValues[4] = { tint.r, tint.g, tint.b, tint.a };
		if (ImGui::ColorEdit4("Tint", tintValues)) { tileLayer->SetTint({ tintValues[0], tintValues[1], tintValues[2], tintValues[3] }); changed = true; }

		bool flipX = tileLayer->GetDefaultFlipX();
		if (ImGui::Checkbox("Flip X", &flipX)) { tileLayer->SetDefaultFlipX(flipX); if (state.setTilesDefaults) needRebuild = true; changed = true; }

		bool flipY = tileLayer->GetDefaultFlipY();
		if (ImGui::Checkbox("Flip Y", &flipY)) { tileLayer->SetDefaultFlipY(flipY); if (state.setTilesDefaults) needRebuild = true; changed = true; }

		bool visible = tileLayer->GetDefaultVisible();
		if (ImGui::Checkbox("Visible", &visible)) { tileLayer->SetDefaultVisible(visible); if (state.setTilesDefaults) needRebuild = true; changed = true; }

		auto& tiles = tileLayer->GetTiles();
		ImGui::Text("Tile Count: %d", (int)tiles.size());
		ImGui::TextUnformatted("Per-tile editor is temporary. Later this should move to a tile tool.");

		if (ImGui::TreeNode("Tiles"))
		{
			for (int i = 0; i < static_cast<int>(tiles.size()); ++i)
			{
				ImGui::PushID(i);
				TileDrawData& tile = tiles[i];
				if (ImGui::TreeNode(("Tile " + std::to_string(i)).c_str()))
				{
					float localPos[2] = { tile.localPosition.x, tile.localPosition.y };
					if (ImGui::DragFloat2("Local Position", localPos, 0.1f, 0.0f, 0.0f, "%.3f")) { tile.localPosition = { localPos[0], localPos[1] }; changed = true; }

					float size[2] = { tile.size.x, tile.size.y };
					if (ImGui::DragFloat2("Size", size, 0.1f, 0.0f, 0.0f, "%.3f")) { tile.size = { size[0], size[1] }; changed = true; }

					int src[4] = { (int)tile.sourceRectPixels.x, (int)tile.sourceRectPixels.y, (int)tile.sourceRectPixels.w, (int)tile.sourceRectPixels.h };
					if (ImGui::DragInt4("Source Rect", src, 1, 0, 0)) { tile.sourceRectPixels = { (float)src[0], (float)src[1], (float)src[2], (float)src[3] }; changed = true; }

					float tileColor[4] = { tile.color.r, tile.color.g, tile.color.b, tile.color.a };
					if (ImGui::ColorEdit4("Color", tileColor)) { tile.color = { tileColor[0], tileColor[1], tileColor[2], tileColor[3] }; changed = true; }

					if (ImGui::Checkbox("Flip X", &tile.flipX)) changed = true;
					if (ImGui::Checkbox("Flip Y", &tile.flipY)) changed = true;
					if (ImGui::Checkbox("Visible", &tile.visible)) changed = true;

					ImGui::TreePop();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	if (needRebuild)
	{
		for (TileDrawData& tile : tileLayer->GetTiles())
		{
			tile.sourceRectPixels = tileLayer->GetDefaultSourceRectPixels();
			tile.size = tileLayer->GetDefaultTileSize();
			tile.flipX = tileLayer->GetDefaultFlipX();
			tile.flipY = tileLayer->GetDefaultFlipY();
			tile.visible = tileLayer->GetDefaultVisible();
		}
	}
	return changed;
}

bool buki::InspectorPanel::DrawRigidBodyComponent(Component* cmp)
{
	RigidBody* rigidBody = dynamic_cast<RigidBody*>(cmp);
	if (rigidBody == nullptr) return false;
	bool changed = false;
	if (ImGui::TreeNodeEx("RigidBody", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TextDisabled("Applied on scene/physics rebuild.");
		ImGui::Separator();

		const char* bodyTypeItems[] = { "Static", "Kinematic", "Dynamic" };
		int currentType = static_cast<int>(rigidBody->def.type);
		if (ImGui::Combo("Body Type", &currentType, bodyTypeItems, IM_ARRAYSIZE(bodyTypeItems))) { rigidBody->def.type = static_cast<RigidBody::BodyType>(currentType); changed = true; }

		if (ImGui::Checkbox("Lock Linear X", &rigidBody->def.motionLocks.linearX)) changed = true;
		if (ImGui::Checkbox("Lock Linear Y", &rigidBody->def.motionLocks.linearY)) changed = true;
		if (ImGui::Checkbox("Lock Angular Z", &rigidBody->def.motionLocks.angularZ)) changed = true;

		ImGui::TreePop();
	}
	return changed;
}

bool buki::InspectorPanel::DrawShapeCommonFields(ShapeDef& def)
{
	bool changed = false;

	if (ImGui::Checkbox("Fill Draw", &def.fillDraw)) changed = true;
	if (ImGui::Checkbox("Shape Draw", &def.shapeDraw)) changed = true;

	float shapeColor[4] = { def.shapeColor.r, def.shapeColor.g, def.shapeColor.b, def.shapeColor.a };
	if (ImGui::ColorEdit4("Shape Color", shapeColor)) { def.shapeColor = { shapeColor[0], shapeColor[1], shapeColor[2], shapeColor[3] }; changed = true; }

	float fillColor[4] = { def.fillColor.r, def.fillColor.g, def.fillColor.b, def.fillColor.a };
	if (ImGui::ColorEdit4("Fill Color", fillColor)) { def.fillColor = { fillColor[0], fillColor[1], fillColor[2], fillColor[3] }; changed = true; }

	if (ImGui::Checkbox("Is Sensor", &def.isSensor)) changed = true;
	if (ImGui::InputInt("Filter", &def.filter)) changed = true;

	if (ImGui::InputFloat("Density", &def.density, 0.0f, 0.0f, "%.3f")) { def.density = ClampMinFloat(def.density, 0.0f); changed = true; }
	if (ImGui::InputFloat("Friction", &def.friction, 0.0f, 0.0f, "%.3f")) { def.friction = ClampFloat(def.friction, 0.0f, 1.0f); changed = true; }
	if (ImGui::InputFloat("Restitution", &def.restitution, 0.0f, 0.0f, "%.3f")) { def.restitution = ClampFloat(def.restitution, 0.0f, 1.0f); changed = true; }

	float positionOffset[2] = { def.positionOffset.x, def.positionOffset.y };
	if (ImGui::InputFloat2("Position Offset", positionOffset, "%.3f")) { def.positionOffset = { positionOffset[0], positionOffset[1] }; changed = true; }

	return changed;
}

bool buki::InspectorPanel::DrawBoxComponent(Component* cmp)
{
	Box* box = dynamic_cast<Box*>(cmp);
	if (box == nullptr) return false;
	bool changed = false;
	if (ImGui::TreeNodeEx("Box", ImGuiTreeNodeFlags_DefaultOpen))
	{
		changed |= DrawShapeCommonFields(box->def);
		float size[2] = { box->def.size.x, box->def.size.y };
		if (ImGui::InputFloat2("Size", size, "%.3f")) { box->def.size = ClampMinVector2({ size[0], size[1] }, 0.001f); changed = true; }
		ImGui::TreePop();
	}
	return changed;
}

bool buki::InspectorPanel::DrawCircleComponent(Component* cmp)
{
	Circle* circle = dynamic_cast<Circle*>(cmp);
	if (circle == nullptr) return false;
	bool changed = false;
	if (ImGui::TreeNodeEx("Circle", ImGuiTreeNodeFlags_DefaultOpen))
	{
		changed |= DrawShapeCommonFields(circle->def);
		if (ImGui::InputFloat("Radius", &circle->def.radius, 0.0f, 0.0f, "%.3f")) { circle->def.radius = ClampMinFloat(circle->def.radius, 0.001f); changed = true; }
		ImGui::TreePop();
	}
	return changed;
}

bool buki::InspectorPanel::DrawPolygonComponent(Component* cmp)
{
	Polygon* polygon = dynamic_cast<Polygon*>(cmp);
	if (polygon == nullptr) return false;
	bool changed = false;
	if (ImGui::TreeNodeEx("Polygon", ImGuiTreeNodeFlags_DefaultOpen))
	{
		changed |= DrawShapeCommonFields(polygon->def);
		if (ImGui::InputFloat("Radius", &polygon->def.radius, 0.0f, 0.0f, "%.3f")) { polygon->def.radius = ClampMinFloat(polygon->def.radius, 0.001f); changed = true; }
		if (ImGui::InputInt("Segments", &polygon->def.segments)) { polygon->def.segments = ClampMinInt(polygon->def.segments, 3); changed = true; }
		ImGui::TreePop();
	}
	return changed;
}

bool buki::InspectorPanel::DrawSharedTextComponents(const std::vector<Entity*>& entities)
{
	std::vector<Text*> texts = GatherSharedComponents<Text>(entities, "Text");
	if (texts.empty())
		return false;

	bool changed = false;

	std::vector<std::string> contents;
	std::vector<std::string> fontPaths;
	std::vector<int> fontSizes;
	std::vector<bool> centerXs;
	std::vector<bool> centerYs;
	std::vector<Color> colors;
	std::vector<Vector2> offsets;

	for (Text* text : texts)
	{
		contents.push_back(text->GetText());
		fontPaths.push_back(text->GetFontPath());
		fontSizes.push_back(*text->GetFontSizeRef());
		centerXs.push_back(*text->GetCenterXRef());
		centerYs.push_back(*text->GetCenterYRef());
		colors.push_back(text->GetColor());
		offsets.push_back(text->GetPositionOffset());
	}

	std::string contentValue = contents[0];
	if (DrawMixedStringField("Content", !AreAllEqualString(contents), contentValue))
	{
		for (Text* text : texts)
			text->SetText(contentValue);
		changed = true;
	}

	std::string fontPathValue = fontPaths[0];
	if (DrawMixedStringField("Font Path", !AreAllEqualString(fontPaths), fontPathValue))
	{
		for (Text* text : texts)
			text->SetFontPath(fontPathValue);
		changed = true;
	}

	int fontSize = fontSizes[0];
	if (DrawMixedIntField("Font Size", !AreAllEqualInt(fontSizes), fontSize))
	{
		for (Text* text : texts)
			*text->GetFontSizeRef() = fontSize;
		changed = true;
	}

	bool centerX = centerXs[0];
	if (DrawMixedBoolField("Center X", !AreAllEqualBool(centerXs), centerX))
	{
		for (Text* text : texts)
			*text->GetCenterXRef() = centerX;
		changed = true;
	}

	bool centerY = centerYs[0];
	if (DrawMixedBoolField("Center Y", !AreAllEqualBool(centerYs), centerY))
	{
		for (Text* text : texts)
			*text->GetCenterYRef() = centerY;
		changed = true;
	}

	Color color = colors[0];
	float colorValues[4] = { color.r, color.g, color.b, color.a };
	if (DrawMixedColorField("Color", !AreAllEqualColor(colors), colorValues))
	{
		Color newColor{ colorValues[0], colorValues[1], colorValues[2], colorValues[3] };
		for (Text* text : texts)
			text->SetColor(newColor);
		changed = true;
	}

	Vector2 offset = offsets[0];
	float offsetX = offset.x;
	float offsetY = offset.y;
	bool changedOffsetX = false;
	bool changedOffsetY = false;
	if (DrawMixedVector2FieldPerAxis(
		"Offset",
		!AreAllEqualVector2X(offsets),
		!AreAllEqualVector2Y(offsets),
		offsetX,
		offsetY,
		changedOffsetX,
		changedOffsetY,
		"%.3f"))
	{
		for (Text* text : texts)
		{
			Vector2 value = text->GetPositionOffset();
			if (changedOffsetX) value.x = offsetX;
			if (changedOffsetY) value.y = offsetY;
			text->SetPositionOffset(value);
		}
		changed = true;
	}

	return changed;
}

bool buki::InspectorPanel::DrawSharedButtonComponents(const std::vector<Entity*>& entities)
{
	std::vector<Button*> buttons = GatherSharedComponents<Button>(entities, "Button");
	if (buttons.empty())
		return false;

	bool changed = false;

	std::vector<std::string> texts;
	std::vector<std::string> messages;
	std::vector<std::string> fontPaths;
	std::vector<int> fontSizes;
	std::vector<bool> centerXs;
	std::vector<bool> centerYs;
	std::vector<bool> fitToTexts;
	std::vector<bool> showBackgrounds;
	std::vector<Color> backgrounds;
	std::vector<Color> textColors;

	for (Button* button : buttons)
	{
		texts.push_back(button->GetText());
		messages.push_back(button->GetMessage());
		fontPaths.push_back(button->GetFontPath());
		fontSizes.push_back(button->GetFontSize());
		centerXs.push_back(button->Style().centerTextX);
		centerYs.push_back(button->Style().centerTextY);
		fitToTexts.push_back(button->Style().fitToText);
		showBackgrounds.push_back(button->Style().showBackground);
		backgrounds.push_back(button->Style().backgroundColor);
		textColors.push_back(button->Style().textColor);
	}

	std::string textValue = texts[0];
	if (DrawMixedStringField("Text", !AreAllEqualString(texts), textValue))
	{
		for (Button* button : buttons)
			button->SetText(textValue);
		changed = true;
	}

	std::string messageValue = messages[0];
	if (DrawMixedStringField("Message", !AreAllEqualString(messages), messageValue))
	{
		for (Button* button : buttons)
			button->SetMessage(messageValue);
		changed = true;
	}

	std::string fontPathValue = fontPaths[0];
	if (DrawMixedStringField("Font Path", !AreAllEqualString(fontPaths), fontPathValue))
	{
		for (Button* button : buttons)
			button->SetFontPath(fontPathValue);
		changed = true;
	}

	int fontSize = fontSizes[0];
	if (DrawMixedIntField("Font Size", !AreAllEqualInt(fontSizes), fontSize))
	{
		fontSize = ClampMinInt(fontSize, 1);
		for (Button* button : buttons)
			button->SetFontSize(fontSize);
		changed = true;
	}

	bool centerX = centerXs[0];
	if (DrawMixedBoolField("Center Text X", !AreAllEqualBool(centerXs), centerX))
	{
		for (Button* button : buttons)
			button->Style().centerTextX = centerX;
		changed = true;
	}

	bool centerY = centerYs[0];
	if (DrawMixedBoolField("Center Text Y", !AreAllEqualBool(centerYs), centerY))
	{
		for (Button* button : buttons)
			button->Style().centerTextY = centerY;
		changed = true;
	}

	bool fitToText = fitToTexts[0];
	if (DrawMixedBoolField("Fit To Text", !AreAllEqualBool(fitToTexts), fitToText))
	{
		for (Button* button : buttons)
			button->Style().fitToText = fitToText;
		changed = true;
	}

	bool showBackground = showBackgrounds[0];
	if (DrawMixedBoolField("Show Background", !AreAllEqualBool(showBackgrounds), showBackground))
	{
		for (Button* button : buttons)
			button->Style().showBackground = showBackground;
		changed = true;
	}

	Color bg = backgrounds[0];
	float bgValues[4] = { bg.r, bg.g, bg.b, bg.a };
	if (DrawMixedColorField("Background", !AreAllEqualColor(backgrounds), bgValues))
	{
		Color newColor{ bgValues[0], bgValues[1], bgValues[2], bgValues[3] };
		for (Button* button : buttons)
			button->Style().backgroundColor = newColor;
		changed = true;
	}

	Color textColor = textColors[0];
	float textColorValues[4] = { textColor.r, textColor.g, textColor.b, textColor.a };
	if (DrawMixedColorField("Text Color", !AreAllEqualColor(textColors), textColorValues))
	{
		Color newColor{ textColorValues[0], textColorValues[1], textColorValues[2], textColorValues[3] };
		for (Button* button : buttons)
			button->Style().textColor = newColor;
		changed = true;
	}

	if (changed)
	{
		for (Button* button : buttons)
			button->Set();
		Engine::Get().Log().LogMessage("Shared Button components updated.");
	}

	return changed;
}

bool buki::InspectorPanel::DrawSharedSpriteComponents(const std::vector<Entity*>& entities)
{
	std::vector<Sprite*> sprites = GatherSharedComponents<Sprite>(entities, "Sprite");
	if (sprites.empty())
		return false;

	bool changed = false;

	std::vector<std::string> paths;
	std::vector<Color> colors;
	std::vector<Vector2> positionOffsets;
	std::vector<Vector2> sizeOffsets;
	std::vector<bool> flipXs;
	std::vector<bool> flipYs;
	std::vector<bool> useSourceRects;
	std::vector<RectF> sourceRects;

	for (Sprite* sprite : sprites)
	{
		paths.push_back(sprite->GetPath());
		colors.push_back(sprite->GetColor());
		positionOffsets.push_back(sprite->GetPositionOffset());
		sizeOffsets.push_back(sprite->GetSizeOffset());
		flipXs.push_back(sprite->GetFlipX());
		flipYs.push_back(sprite->GetFlipY());
		useSourceRects.push_back(sprite->UsesSourceRect());
		sourceRects.push_back(sprite->GetSourceRectPixels());
	}

	std::vector<std::string> pathCopies;
	std::vector<std::string*> pathRefs;
	for (Sprite* sprite : sprites)
	{
		pathCopies.push_back(sprite->GetPath());
	}
	for (std::string& p : pathCopies)
	{
		pathRefs.push_back(&p);
	}

	if (DrawSharedImageAssetPathPicker("Texture Path", pathRefs))
	{
		for (size_t i = 0; i < sprites.size(); ++i)
			sprites[i]->SetPath(pathCopies[i]);
		changed = true;
	}

	Color color = colors[0];
	float colorValues[4] = { color.r, color.g, color.b, color.a };
	if (DrawMixedColorField("Color", !AreAllEqualColor(colors), colorValues))
	{
		Color newColor{ colorValues[0], colorValues[1], colorValues[2], colorValues[3] };
		for (Sprite* sprite : sprites)
			sprite->SetColor(newColor);
		changed = true;
	}

	Vector2 posOffset = positionOffsets[0];
	float posOffsetX = posOffset.x;
	float posOffsetY = posOffset.y;
	bool changedPosOffsetX = false;
	bool changedPosOffsetY = false;
	if (DrawMixedVector2FieldPerAxis(
		"Position Offset",
		!AreAllEqualVector2X(positionOffsets),
		!AreAllEqualVector2Y(positionOffsets),
		posOffsetX,
		posOffsetY,
		changedPosOffsetX,
		changedPosOffsetY,
		"%.3f"))
	{
		for (Sprite* sprite : sprites)
		{
			Vector2 value = sprite->GetPositionOffset();
			if (changedPosOffsetX) value.x = posOffsetX;
			if (changedPosOffsetY) value.y = posOffsetY;
			sprite->SetPositionOffset(value);
		}
		changed = true;
	}

	Vector2 sizeOffset = sizeOffsets[0];
	float sizeOffsetX = sizeOffset.x;
	float sizeOffsetY = sizeOffset.y;
	bool changedSizeOffsetX = false;
	bool changedSizeOffsetY = false;
	if (DrawMixedVector2FieldPerAxis(
		"Size Offset",
		!AreAllEqualVector2X(sizeOffsets),
		!AreAllEqualVector2Y(sizeOffsets),
		sizeOffsetX,
		sizeOffsetY,
		changedSizeOffsetX,
		changedSizeOffsetY,
		"%.3f"))
	{
		for (Sprite* sprite : sprites)
		{
			Vector2 value = sprite->GetSizeOffset();
			if (changedSizeOffsetX) value.x = sizeOffsetX;
			if (changedSizeOffsetY) value.y = sizeOffsetY;
			sprite->SetSizeOffset(value);
		}
		changed = true;
	}

	bool flipX = flipXs[0];
	if (DrawMixedBoolField("Flip X", !AreAllEqualBool(flipXs), flipX))
	{
		for (Sprite* sprite : sprites)
			sprite->SetFlipX(flipX);
		changed = true;
	}

	bool flipY = flipYs[0];
	if (DrawMixedBoolField("Flip Y", !AreAllEqualBool(flipYs), flipY))
	{
		for (Sprite* sprite : sprites)
			sprite->SetFlipY(flipY);
		changed = true;
	}

	bool useSourceRect = useSourceRects[0];
	if (DrawMixedBoolField("Use Source Rect", !AreAllEqualBool(useSourceRects), useSourceRect))
	{
		for (Sprite* sprite : sprites)
			sprite->SetUseSourceRect(useSourceRect);
		changed = true;
	}

	RectF src = sourceRects[0];
	float srcValues[4] = { src.x, src.y, src.w, src.h };
	if (DrawMixedFloat4Field("Source Rect", !AreAllEqualRectF(sourceRects), srcValues, "%.3f"))
	{
		RectF newRect{ srcValues[0], srcValues[1], srcValues[2], srcValues[3] };
		for (Sprite* sprite : sprites)
			sprite->SetSourceRectPixels(newRect);
		changed = true;
	}

	return changed;
}

bool buki::InspectorPanel::DrawSharedRigidBodyComponents(const std::vector<Entity*>& entities)
{
	std::vector<RigidBody*> bodies = GatherSharedComponents<RigidBody>(entities, "RigidBody");
	if (bodies.empty())
		return false;

	bool changed = false;

	std::vector<int> bodyTypes;
	std::vector<bool> lockLinearXs;
	std::vector<bool> lockLinearYs;
	std::vector<bool> lockAngularZs;

	for (RigidBody* body : bodies)
	{
		bodyTypes.push_back(static_cast<int>(body->def.type));
		lockLinearXs.push_back(body->def.motionLocks.linearX);
		lockLinearYs.push_back(body->def.motionLocks.linearY);
		lockAngularZs.push_back(body->def.motionLocks.angularZ);
	}

	ImGui::TextDisabled("Applied on scene/physics rebuild.");
	ImGui::Separator();

	const char* bodyTypeItems[] = { "Static", "Kinematic", "Dynamic" };
	int currentType = bodyTypes[0];
	if (DrawMixedComboField("Body Type", !AreAllEqualInt(bodyTypes), currentType, bodyTypeItems, IM_ARRAYSIZE(bodyTypeItems)))
	{
		for (RigidBody* body : bodies)
			body->def.type = static_cast<RigidBody::BodyType>(currentType);
		changed = true;
	}

	bool lockLinearX = lockLinearXs[0];
	if (DrawMixedBoolField("Lock Linear X", !AreAllEqualBool(lockLinearXs), lockLinearX))
	{
		for (RigidBody* body : bodies)
			body->def.motionLocks.linearX = lockLinearX;
		changed = true;
	}

	bool lockLinearY = lockLinearYs[0];
	if (DrawMixedBoolField("Lock Linear Y", !AreAllEqualBool(lockLinearYs), lockLinearY))
	{
		for (RigidBody* body : bodies)
			body->def.motionLocks.linearY = lockLinearY;
		changed = true;
	}

	bool lockAngularZ = lockAngularZs[0];
	if (DrawMixedBoolField("Lock Angular Z", !AreAllEqualBool(lockAngularZs), lockAngularZ))
	{
		for (RigidBody* body : bodies)
			body->def.motionLocks.angularZ = lockAngularZ;
		changed = true;
	}

	return changed;
}

bool buki::InspectorPanel::DrawSharedShapeCommonFields(const std::vector<ShapeDef*>& defs)
{
	if (defs.empty())
		return false;

	bool changed = false;

	std::vector<bool> fillDraws;
	std::vector<bool> shapeDraws;
	std::vector<Color> shapeColors;
	std::vector<Color> fillColors;
	std::vector<bool> isSensors;
	std::vector<int> filters;
	std::vector<float> densities;
	std::vector<float> frictions;
	std::vector<float> restitutions;
	std::vector<Vector2> positionOffsets;

	for (ShapeDef* def : defs)
	{
		fillDraws.push_back(def->fillDraw);
		shapeDraws.push_back(def->shapeDraw);
		shapeColors.push_back(def->shapeColor);
		fillColors.push_back(def->fillColor);
		isSensors.push_back(def->isSensor);
		filters.push_back(def->filter);
		densities.push_back(def->density);
		frictions.push_back(def->friction);
		restitutions.push_back(def->restitution);
		positionOffsets.push_back(def->positionOffset);
	}

	bool fillDraw = fillDraws[0];
	if (DrawMixedBoolField("Fill Draw", !AreAllEqualBool(fillDraws), fillDraw))
	{
		for (ShapeDef* def : defs)
			def->fillDraw = fillDraw;
		changed = true;
	}

	bool shapeDraw = shapeDraws[0];
	if (DrawMixedBoolField("Shape Draw", !AreAllEqualBool(shapeDraws), shapeDraw))
	{
		for (ShapeDef* def : defs)
			def->shapeDraw = shapeDraw;
		changed = true;
	}

	Color shapeColor = shapeColors[0];
	float shapeColorValues[4] = { shapeColor.r, shapeColor.g, shapeColor.b, shapeColor.a };
	if (DrawMixedColorField("Shape Color", !AreAllEqualColor(shapeColors), shapeColorValues))
	{
		Color newColor{ shapeColorValues[0], shapeColorValues[1], shapeColorValues[2], shapeColorValues[3] };
		for (ShapeDef* def : defs)
			def->shapeColor = newColor;
		changed = true;
	}

	Color fillColor = fillColors[0];
	float fillColorValues[4] = { fillColor.r, fillColor.g, fillColor.b, fillColor.a };
	if (DrawMixedColorField("Fill Color", !AreAllEqualColor(fillColors), fillColorValues))
	{
		Color newColor{ fillColorValues[0], fillColorValues[1], fillColorValues[2], fillColorValues[3] };
		for (ShapeDef* def : defs)
			def->fillColor = newColor;
		changed = true;
	}

	bool isSensor = isSensors[0];
	if (DrawMixedBoolField("Is Sensor", !AreAllEqualBool(isSensors), isSensor))
	{
		for (ShapeDef* def : defs)
			def->isSensor = isSensor;
		changed = true;
	}

	int filter = filters[0];
	if (DrawMixedIntField("Filter", !AreAllEqualInt(filters), filter))
	{
		for (ShapeDef* def : defs)
			def->filter = filter;
		changed = true;
	}

	float density = densities[0];
	if (DrawMixedFloatField("Density", !AreAllEqualFloat(densities), density, "%.3f"))
	{
		density = ClampMinFloat(density, 0.0f);
		for (ShapeDef* def : defs)
			def->density = density;
		changed = true;
	}

	float friction = frictions[0];
	if (DrawMixedFloatField("Friction", !AreAllEqualFloat(frictions), friction, "%.3f"))
	{
		friction = ClampFloat(friction, 0.0f, 1.0f);
		for (ShapeDef* def : defs)
			def->friction = friction;
		changed = true;
	}

	float restitution = restitutions[0];
	if (DrawMixedFloatField("Restitution", !AreAllEqualFloat(restitutions), restitution, "%.3f"))
	{
		restitution = ClampFloat(restitution, 0.0f, 1.0f);
		for (ShapeDef* def : defs)
			def->restitution = restitution;
		changed = true;
	}

	Vector2 positionOffset = positionOffsets[0];
	float positionOffsetX = positionOffset.x;
	float positionOffsetY = positionOffset.y;
	bool changedPositionOffsetX = false;
	bool changedPositionOffsetY = false;
	if (DrawMixedVector2FieldPerAxis(
		"Position Offset",
		!AreAllEqualVector2X(positionOffsets),
		!AreAllEqualVector2Y(positionOffsets),
		positionOffsetX,
		positionOffsetY,
		changedPositionOffsetX,
		changedPositionOffsetY,
		"%.3f"))
	{
		for (ShapeDef* def : defs)
		{
			Vector2 value = def->positionOffset;
			if (changedPositionOffsetX) value.x = positionOffsetX;
			if (changedPositionOffsetY) value.y = positionOffsetY;
			def->positionOffset = value;
		}
		changed = true;
	}

	return changed;
}

bool buki::InspectorPanel::DrawSharedBoxComponents(const std::vector<Entity*>& entities)
{
	std::vector<Box*> boxes = GatherSharedComponents<Box>(entities, "Box");
	if (boxes.empty())
		return false;

	bool changed = false;

	std::vector<ShapeDef*> defs;
	std::vector<Vector2> sizes;

	for (Box* box : boxes)
	{
		defs.push_back(&box->def);
		sizes.push_back(box->def.size);
	}

	changed |= DrawSharedShapeCommonFields(defs);

	Vector2 size = sizes[0];
	float sizeX = size.x;
	float sizeY = size.y;
	bool changedSizeX = false;
	bool changedSizeY = false;
	if (DrawMixedVector2FieldPerAxis(
		"Size",
		!AreAllEqualVector2X(sizes),
		!AreAllEqualVector2Y(sizes),
		sizeX,
		sizeY,
		changedSizeX,
		changedSizeY,
		"%.3f"))
	{
		for (Box* box : boxes)
		{
			Vector2 value = box->def.size;
			if (changedSizeX) value.x = sizeX;
			if (changedSizeY) value.y = sizeY;
			box->def.size = ClampMinVector2(value, 0.001f);
		}
		changed = true;
	}

	return changed;
}

bool buki::InspectorPanel::DrawSharedCircleComponents(const std::vector<Entity*>& entities)
{
	std::vector<Circle*> circles = GatherSharedComponents<Circle>(entities, "Circle");
	if (circles.empty())
		return false;

	bool changed = false;

	std::vector<ShapeDef*> defs;
	std::vector<float> radii;

	for (Circle* circle : circles)
	{
		defs.push_back(&circle->def);
		radii.push_back(circle->def.radius);
	}

	changed |= DrawSharedShapeCommonFields(defs);

	float radius = radii[0];
	if (DrawMixedFloatField("Radius", !AreAllEqualFloat(radii), radius, "%.3f"))
	{
		radius = ClampMinFloat(radius, 0.001f);
		for (Circle* circle : circles)
			circle->def.radius = radius;
		changed = true;
	}

	return changed;
}

bool buki::InspectorPanel::DrawSharedPolygonComponents(const std::vector<Entity*>& entities)
{
	std::vector<Polygon*> polygons = GatherSharedComponents<Polygon>(entities, "Polygon");
	if (polygons.empty())
		return false;

	bool changed = false;

	std::vector<ShapeDef*> defs;
	std::vector<float> radii;
	std::vector<int> segments;

	for (Polygon* polygon : polygons)
	{
		defs.push_back(&polygon->def);
		radii.push_back(polygon->def.radius);
		segments.push_back(polygon->def.segments);
	}

	changed |= DrawSharedShapeCommonFields(defs);

	float radius = radii[0];
	if (DrawMixedFloatField("Radius", !AreAllEqualFloat(radii), radius, "%.3f"))
	{
		radius = ClampMinFloat(radius, 0.001f);
		for (Polygon* polygon : polygons)
			polygon->def.radius = radius;
		changed = true;
	}

	int segmentCount = segments[0];
	if (DrawMixedIntField("Segments", !AreAllEqualInt(segments), segmentCount))
	{
		segmentCount = ClampMinInt(segmentCount, 3);
		for (Polygon* polygon : polygons)
			polygon->def.segments = segmentCount;
		changed = true;
	}

	return changed;
}

std::string buki::InspectorPanel::RemoveComponent(Component* cmp)
{
	std::string name = ComponentFactory::GetTypeName(typeid(cmp));
	if (!ImGui::Button(("Remove " + name).c_str()))
		return "";
	return name;
}

bool buki::InspectorPanel::DrawImageAssetPathPicker(const char* label, std::string& path)
{
	bool changed = false;

	ImGui::PushID(label);

	std::string displayName = path.empty() ? "" : DisplayNameForAssetPath(path);
	char buffer[512] = {};
	std::snprintf(buffer, sizeof(buffer), "%s", displayName.c_str());

	ImGui::TextUnformatted(label);
	ImGui::SameLine();

	ImGui::SetNextItemWidth(260.0f);
	ImGui::InputText("##Value", buffer, sizeof(buffer), ImGuiInputTextFlags_ReadOnly);

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMAGE"))
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
	if (ImGui::Button("Pick"))
	{
		ImGui::OpenPopup("ImagePickerPopup");
	}

	if (ImGui::BeginPopup("ImagePickerPopup"))
	{
		static char filterBuffer[256] = {};
		ImGui::InputText("Search", filterBuffer, sizeof(filterBuffer));

		ImGui::Separator();

		if (ImGui::Selectable("(None)"))
		{
			path.clear();
			changed = true;
			ImGui::CloseCurrentPopup();
		}

		const std::vector<std::string> assets = CollectImageAssets("../Deployment/assets");
		const std::string filter = ToLowerCopy(filterBuffer);

		for (const std::string& asset : assets)
		{
			const std::string filename = DisplayNameForAssetPath(asset);
			const std::string folderSuffix = FolderSuffixForAssetPath(asset);

			std::string searchable = ToLowerCopy(filename + " " + folderSuffix + " " + asset);
			if (!filter.empty() && searchable.find(filter) == std::string::npos)
				continue;

			ImGui::PushID(asset.c_str());

			bool selected = (asset == path);
			if (ImGui::Selectable(filename.c_str(), selected))
			{
				path = asset;
				changed = true;
				ImGui::CloseCurrentPopup();
			}

			if (!folderSuffix.empty())
			{
				ImGui::SameLine();
				ImGui::TextDisabled("%s", folderSuffix.c_str());
			}

			ImGui::PopID();
		}

		ImGui::EndPopup();
	}

	ImGui::PopID();
	return changed;
}

bool buki::InspectorPanel::DrawSharedImageAssetPathPicker(const char* label, std::vector<std::string*>& paths)
{
	if (paths.empty())
		return false;

	bool changed = false;

	bool mixed = false;
	const std::string first = (paths[0] != nullptr) ? *paths[0] : "";

	for (size_t i = 1; i < paths.size(); ++i)
	{
		if (paths[i] == nullptr)
			continue;

		if (*paths[i] != first)
		{
			mixed = true;
			break;
		}
	}

	std::string displayValue = mixed ? "-" : DisplayNameForAssetPath(first);

	ImGui::PushID(label);

	if (DrawMixedStringField(label, mixed, displayValue))
	{
		const std::string typed = displayValue;

		if (typed.empty())
		{
			for (std::string* path : paths)
			{
				if (path != nullptr)
					path->clear();
			}
			changed = true;
		}
		else if (typed != "-")
		{
			const std::vector<std::string> assets = CollectImageAssets("../Deployment/assets");

			for (const std::string& asset : assets)
			{
				const std::string filename = DisplayNameForAssetPath(asset);
				if (filename == typed)
				{
					for (std::string* path : paths)
					{
						if (path != nullptr)
							*path = asset;
					}
					changed = true;
					break;
				}
			}
		}
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMAGE"))
		{
			const char* dropped = static_cast<const char*>(payload->Data);
			if (dropped != nullptr)
			{
				for (std::string* path : paths)
				{
					if (path != nullptr)
						*path = dropped;
				}
				changed = true;
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	if (ImGui::Button("Pick"))
	{
		ImGui::OpenPopup("ImagePickerPopup");
	}

	if (ImGui::BeginPopup("ImagePickerPopup"))
	{
		static char filterBuffer[256] = {};
		ImGui::InputText("Search", filterBuffer, sizeof(filterBuffer));

		ImGui::Separator();

		if (ImGui::Selectable("(None)"))
		{
			for (std::string* path : paths)
			{
				if (path != nullptr)
					path->clear();
			}
			changed = true;
			ImGui::CloseCurrentPopup();
		}

		const std::vector<std::string> assets = CollectImageAssets("../Deployment/assets");
		const std::string filter = ToLowerCopy(filterBuffer);

		for (const std::string& asset : assets)
		{
			const std::string filename = DisplayNameForAssetPath(asset);
			const std::string folderSuffix = FolderSuffixForAssetPath(asset);

			std::string searchable = ToLowerCopy(filename + " " + folderSuffix + " " + asset);
			if (!filter.empty() && searchable.find(filter) == std::string::npos)
				continue;

			ImGui::PushID(asset.c_str());

			bool selected = (!mixed && asset == first);
			if (ImGui::Selectable(filename.c_str(), selected))
			{
				for (std::string* path : paths)
				{
					if (path != nullptr)
						*path = asset;
				}
				changed = true;
				ImGui::CloseCurrentPopup();
			}

			if (!folderSuffix.empty())
			{
				ImGui::SameLine();
				ImGui::TextDisabled("%s", folderSuffix.c_str());
			}

			ImGui::PopID();
		}

		ImGui::EndPopup();
	}

	ImGui::PopID();
	return changed;
}



bool buki::InspectorPanel::DrawMixedStringField(const char* label, bool mixed, std::string& value)
{
	bool changed = false;

	ImGui::PushID(label);

	ImGuiID fieldId = ImGui::GetID("MixedStringField");
	const std::string initialDisplay = mixed ? "-" : value;
	MixedTextState& state = GetMixedTextState(fieldId, initialDisplay);

	char buffer[512] = {};
	std::snprintf(buffer, sizeof(buffer), "%s", state.value.c_str());

	ImGui::TextUnformatted(label);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(260.0f);

	if (ImGui::InputText("##MixedStringField", buffer, sizeof(buffer)))
	{
		state.value = buffer;
		value = buffer;
		changed = true;
	}

	if (!ImGui::IsItemActive())
	{
		ResetMixedTextState(fieldId);
	}

	ImGui::PopID();
	return changed;
}

bool buki::InspectorPanel::DrawMixedIntField(const char* label, bool mixed, int& value)
{
	bool changed = false;

	ImGui::PushID(label);

	ImGuiID fieldId = ImGui::GetID("MixedIntField");
	const std::string initialDisplay = mixed ? "-" : std::to_string(value);
	MixedTextState& state = GetMixedTextState(fieldId, initialDisplay);

	char buffer[64] = {};
	std::snprintf(buffer, sizeof(buffer), "%s", state.value.c_str());

	ImGui::TextUnformatted(label);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(260.0f);

	if (ImGui::InputText("##MixedIntField", buffer, sizeof(buffer), ImGuiInputTextFlags_CharsDecimal))
	{
		state.value = buffer;
		if (state.value != "-" && !state.value.empty())
		{
			char* end = nullptr;
			long parsed = std::strtol(state.value.c_str(), &end, 10);
			if (end != state.value.c_str() && *end == '\0')
			{
				value = static_cast<int>(parsed);
				changed = true;
			}
		}
	}

	if (!ImGui::IsItemActive())
	{
		ResetMixedTextState(fieldId);
	}

	ImGui::PopID();
	return changed;
}

bool buki::InspectorPanel::DrawMixedFloatField(const char* label, bool mixed, float& value, const char* format)
{
	bool changed = false;

	ImGui::PushID(label);

	ImGuiID fieldId = ImGui::GetID("MixedFloatField");

	char formatted[64] = {};
	std::snprintf(formatted, sizeof(formatted), format, value);

	const std::string initialDisplay = mixed ? "-" : formatted;
	MixedTextState& state = GetMixedTextState(fieldId, initialDisplay);

	char buffer[64] = {};
	std::snprintf(buffer, sizeof(buffer), "%s", state.value.c_str());

	ImGui::TextUnformatted(label);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(260.0f);

	if (ImGui::InputText("##MixedFloatField", buffer, sizeof(buffer)))
	{
		state.value = buffer;
		if (state.value != "-" && !state.value.empty())
		{
			char* end = nullptr;
			float parsed = std::strtof(state.value.c_str(), &end);
			if (end != state.value.c_str() && *end == '\0')
			{
				value = parsed;
				changed = true;
			}
		}
	}

	if (!ImGui::IsItemActive())
	{
		ResetMixedTextState(fieldId);
	}

	ImGui::PopID();
	return changed;
}

bool buki::InspectorPanel::DrawMixedBoolField(const char* label, bool mixed, bool& value)
{
	bool changed = false;

	ImGui::PushID(label);

	if (mixed)
	{
		ImGui::TextUnformatted(label);
		ImGui::SameLine();
		ImGui::TextDisabled("-");
		ImGui::SameLine();

		bool temp = value;
		if (ImGui::Checkbox("##MixedBoolField", &temp))
		{
			value = temp;
			changed = true;
		}
	}
	else
	{
		if (ImGui::Checkbox(label, &value))
		{
			changed = true;
		}
	}

	ImGui::PopID();
	return changed;
}

bool buki::InspectorPanel::DrawMixedFloat2Field(const char* label, bool mixed, float values[2], const char* format)
{
	bool changed = false;

	ImGui::PushID(label);

	if (!mixed)
	{
		changed = ImGui::InputFloat2(label, values, format);
		ImGui::PopID();
		return changed;
	}

	ImGuiID idX = ImGui::GetID("##MixedFloat2FieldX");
	ImGuiID idY = ImGui::GetID("##MixedFloat2FieldY");

	MixedTextState& stateX = GetMixedTextState(idX, "-");
	MixedTextState& stateY = GetMixedTextState(idY, "-");

	char bufferX[64] = {};
	char bufferY[64] = {};
	std::snprintf(bufferX, sizeof(bufferX), "%s", stateX.value.c_str());
	std::snprintf(bufferY, sizeof(bufferY), "%s", stateY.value.c_str());

	ImGui::TextUnformatted(label);
	ImGui::SameLine();

	ImGui::SetNextItemWidth(128.0f);
	if (ImGui::InputText("##MixedFloat2FieldX", bufferX, sizeof(bufferX)))
	{
		stateX.value = bufferX;
		if (stateX.value != "-" && !stateX.value.empty())
		{
			char* end = nullptr;
			float parsed = std::strtof(stateX.value.c_str(), &end);
			if (end != stateX.value.c_str() && *end == '\0')
			{
				values[0] = parsed;
				changed = true;
			}
		}
	}
	const bool xActive = ImGui::IsItemActive();

	ImGui::SameLine();

	ImGui::SetNextItemWidth(128.0f);
	if (ImGui::InputText("##MixedFloat2FieldY", bufferY, sizeof(bufferY)))
	{
		stateY.value = bufferY;
		if (stateY.value != "-" && !stateY.value.empty())
		{
			char* end = nullptr;
			float parsed = std::strtof(stateY.value.c_str(), &end);
			if (end != stateY.value.c_str() && *end == '\0')
			{
				values[1] = parsed;
				changed = true;
			}
		}
	}
	const bool yActive = ImGui::IsItemActive();

	if (!xActive && !yActive)
	{
		ResetMixedTextState(idX);
		ResetMixedTextState(idY);
	}

	ImGui::PopID();
	return changed;
}

bool buki::InspectorPanel::DrawMixedFloat4Field(const char* label, bool mixed, float values[4], const char* format)
{
	bool changed = false;

	ImGui::PushID(label);

	if (!mixed)
	{
		changed = ImGui::InputFloat4(label, values, format);
		ImGui::PopID();
		return changed;
	}

	ImGui::TextUnformatted(label);
	ImGui::SameLine();

	bool anyActive = false;

	for (int i = 0; i < 4; ++i)
	{
		if (i > 0)
			ImGui::SameLine();

		std::string fieldIdStr = "##MixedFloat4Field" + std::to_string(i);
		ImGuiID fieldId = ImGui::GetID(fieldIdStr.c_str());
		MixedTextState& state = GetMixedTextState(fieldId, "-");

		char buffer[64] = {};
		std::snprintf(buffer, sizeof(buffer), "%s", state.value.c_str());

		ImGui::SetNextItemWidth(60.0f);
		if (ImGui::InputText(fieldIdStr.c_str(), buffer, sizeof(buffer)))
		{
			state.value = buffer;
			if (state.value != "-" && !state.value.empty())
			{
				char* end = nullptr;
				float parsed = std::strtof(state.value.c_str(), &end);
				if (end != state.value.c_str() && *end == '\0')
				{
					values[i] = parsed;
					changed = true;
				}
			}
		}

		anyActive = anyActive || ImGui::IsItemActive();
	}

	if (!anyActive)
	{
		for (int i = 0; i < 4; ++i)
		{
			std::string fieldIdStr = "##MixedFloat4Field" + std::to_string(i);
			ResetMixedTextState(ImGui::GetID(fieldIdStr.c_str()));
		}
	}

	ImGui::PopID();
	return changed;
}

bool buki::InspectorPanel::DrawMixedColorField(const char* label, bool mixed, float values[4])
{
	bool changed = false;

	ImGui::PushID(label);

	if (mixed)
	{
		ImGui::TextUnformatted(label);
		ImGui::SameLine();
		ImGui::TextDisabled("-");
		ImGui::SameLine();

		changed = ImGui::ColorEdit4("##MixedColorField", values);
	}
	else
	{
		changed = ImGui::ColorEdit4(label, values);
	}

	ImGui::PopID();
	return changed;
}

bool buki::InspectorPanel::DrawMixedComboField(const char* label, bool mixed, int& currentIndex, const char* const items[], int itemsCount)
{
	bool changed = false;

	ImGui::PushID(label);

	const char* preview = mixed ? "-" : items[currentIndex];
	if (ImGui::BeginCombo(label, preview))
	{
		for (int i = 0; i < itemsCount; ++i)
		{
			bool selected = (!mixed && i == currentIndex);
			if (ImGui::Selectable(items[i], selected))
			{
				currentIndex = i;
				changed = true;
			}

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::PopID();
	return changed;
}



bool buki::InspectorPanel::DrawMixedVector2FieldPerAxis(
	const char* label,
	bool mixedX,
	bool mixedY,
	float& x,
	float& y,
	bool& changedX,
	bool& changedY,
	const char* format)
{
	changedX = false;
	changedY = false;

	ImGui::PushID(label);

	char overlayX[64] = {};
	char overlayY[64] = {};

	if (mixedX)
		std::snprintf(overlayX, sizeof(overlayX), "-");
	if (mixedY)
		std::snprintf(overlayY, sizeof(overlayY), "-");

	ImGui::TextUnformatted(label);
	ImGui::SameLine();

	ImGui::SetNextItemWidth(128.0f);
	if (ImGui::DragFloat("##PerAxisX", &x, 0.1f, 0.0f, 0.0f, format))
	{
		changedX = true;
	}

	if (mixedX)
	{
		ImVec2 min = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();
		ImVec2 textSize = ImGui::CalcTextSize(overlayX);
		ImVec2 pos(
			min.x + ImGui::GetStyle().FramePadding.x,
			min.y + (max.y - min.y - textSize.y) * 0.5f);

		ImGui::GetWindowDrawList()->AddText(
			pos,
			ImGui::GetColorU32(ImGuiCol_TextDisabled),
			overlayX);
	}

	ImGui::SameLine();

	ImGui::SetNextItemWidth(128.0f);
	if (ImGui::DragFloat("##PerAxisY", &y, 0.1f, 0.0f, 0.0f, format))
	{
		changedY = true;
	}

	if (mixedY)
	{
		ImVec2 min = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();
		ImVec2 textSize = ImGui::CalcTextSize(overlayY);
		ImVec2 pos(
			min.x + ImGui::GetStyle().FramePadding.x,
			min.y + (max.y - min.y - textSize.y) * 0.5f);

		ImGui::GetWindowDrawList()->AddText(
			pos,
			ImGui::GetColorU32(ImGuiCol_TextDisabled),
			overlayY);
	}

	ImGui::PopID();
	return changedX || changedY;
}

bool buki::InspectorPanel::DrawMixedVector2DragDeltaFieldPerAxis(
	const char* label,
	bool mixedX,
	bool mixedY,
	float& x,
	float& y,
	float& deltaX,
	float& deltaY,
	bool& changedX,
	bool& changedY,
	const char* format)
{
	changedX = false;
	changedY = false;
	deltaX = 0.0f;
	deltaY = 0.0f;

	ImGui::PushID(label);

	float beforeX = x;
	float beforeY = y;

	ImGui::TextUnformatted(label);
	ImGui::SameLine();

	ImGui::SetNextItemWidth(128.0f);
	if (ImGui::DragFloat("##PerAxisX", &x, 0.1f, 0.0f, 0.0f, format))
	{
		changedX = true;
		deltaX = x - beforeX;
	}

	if (mixedX && !ImGui::IsItemActive())
	{
		ImVec2 min = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();
		ImVec2 textSize = ImGui::CalcTextSize("-");
		ImVec2 pos(
			min.x + ImGui::GetStyle().FramePadding.x,
			min.y + (max.y - min.y - textSize.y) * 0.5f);

		ImGui::GetWindowDrawList()->AddText(
			pos,
			ImGui::GetColorU32(ImGuiCol_TextDisabled),
			"-");
	}

	ImGui::SameLine();

	ImGui::SetNextItemWidth(128.0f);
	if (ImGui::DragFloat("##PerAxisY", &y, 0.1f, 0.0f, 0.0f, format))
	{
		changedY = true;
		deltaY = y - beforeY;
	}

	if (mixedY && !ImGui::IsItemActive())
	{
		ImVec2 min = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();
		ImVec2 textSize = ImGui::CalcTextSize("-");
		ImVec2 pos(
			min.x + ImGui::GetStyle().FramePadding.x,
			min.y + (max.y - min.y - textSize.y) * 0.5f);

		ImGui::GetWindowDrawList()->AddText(
			pos,
			ImGui::GetColorU32(ImGuiCol_TextDisabled),
			"-");
	}

	ImGui::PopID();
	return changedX || changedY;
}
