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

#include <fstream>

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
}

void buki::InspectorPanel::Render(EditorState& state)
{
	ImGui::Begin("Inspector", &state.showInspector);

	Entity* entity = state.selectedEntity;
	if (entity == nullptr)
	{
		ImGui::TextUnformatted("No entity selected.");
		ImGui::End();
		return;
	}

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
		if (ImGui::InputText("Texture Path", pathBuffer, sizeof(pathBuffer))) { sprite->SetPath(pathBuffer); changed = true; }
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

std::string buki::InspectorPanel::RemoveComponent(Component* cmp)
{
	std::string name = ComponentFactory::GetTypeName(typeid(cmp));
	if (!ImGui::Button(("Remove " + name).c_str()))
		return "";
	return name;
}
