#pragma once
#include "Component.h"
#include "Engine.h"
#include "nlohmann/json.hpp"
#include "Entity.h"
#include "EntityRef.h"
#include "Button.h"
#include "TileLayer.h"

using json = nlohmann::json;

buki::Component::Component() : Component(nullptr)
{
}

buki::Component::Component(Entity* _entity) : m_Entity(_entity)
{
}

json buki::Component::Serialize() const
{
	json doc = json::object();

	for (const PropertyInfo& propInfo : GetProperties())
	{
		const char* base = reinterpret_cast<const char*>(this);
		auto& fields = propInfo.prop.fields;
		auto& lastField = fields[fields.size() - 1];
		const void* fieldPtr = base + lastField.offset;

		json* current = &doc;
		std::string key = lastField.key;
		if (fields.size() > 1)
		{
			for (size_t i = 0; i < fields.size() - 1; i++)
			{
				const BukiField& field = fields[i];
				if (!current->contains(field.key))
				{
					(*current)[field.key] = json::object();
				}
				current = &(*current)[field.key];
			}
		}



		switch (propInfo.prop.type)
		{
		case PropertyType::Int:
		case PropertyType::EnumInt:
			(*current)[key] = *reinterpret_cast<const int*>(fieldPtr);
			break;

		case PropertyType::Float:
			(*current)[key] = *reinterpret_cast<const float*>(fieldPtr);
			break;

		case PropertyType::Bool:
			(*current)[key] = *reinterpret_cast<const bool*>(fieldPtr);
			break;

		case PropertyType::String:
		case PropertyType::ImageAsset:
		case PropertyType::AudioAsset:
		case PropertyType::FontAsset:
		case PropertyType::PrefabRef:
			(*current)[key] = *reinterpret_cast<const std::string*>(fieldPtr);
			break;

		case PropertyType::StringList:
		case PropertyType::ImageAssetList:
		case PropertyType::AudioAssetList:
		case PropertyType::FontAssetList:
			(*current)[key] = *reinterpret_cast<const std::vector<std::string>*>(fieldPtr);
			break;

		case PropertyType::EntityRef:
		{
			const EntityRef& ref = *reinterpret_cast<const EntityRef*>(fieldPtr);
			(*current)[key] =
			{
				{ "entityName", ref.entityName },
				{ "prefabPath", ref.prefabPath }
			};
			break;
		}

		case PropertyType::Vector2:
		{
			const Vector2& v = *reinterpret_cast<const Vector2*>(fieldPtr);
			(*current)[key]["x"] = v.x;
			(*current)[key]["y"] = v.y;
			break;
		}

		case PropertyType::Color:
		{
			const Color& c = *reinterpret_cast<const Color*>(fieldPtr);
			(*current)[key] =
			{
				{ "r", c.r },
				{ "g", c.g },
				{ "b", c.b },
				{ "a", c.a }
			};
			break;
		}

		case PropertyType::RectF:
		{
			const RectF& r = *reinterpret_cast<const RectF*>(fieldPtr);
			(*current)[key] =
			{
				{ "x", r.x },
				{ "y", r.y },
				{ "w", r.w },
				{ "h", r.h }
			};
			break;
		}
		case PropertyType::EnumScriptBinding:
		{
			const StaticFunctionBinding& b = *reinterpret_cast<const StaticFunctionBinding*>(fieldPtr);
			(*current)[key]["scriptTypeName"] = b.scriptTypeName;
			(*current)[key]["functionName"] = b.functionName;

			break;
		}
		case PropertyType::TileList:
		{
			const std::vector<TileDrawData>& tileList = *reinterpret_cast<const std::vector<TileDrawData>*>(fieldPtr);
			json tileJson;
			for (const TileDrawData& tile : tileList)
			{
				tileJson["localPosition"] = { {"x", tile.localPosition.x}, {"y", tile.localPosition.y} };
				tileJson["size"] = { {"x", tile.size.x}, {"y", tile.size.y} };
				tileJson["sourceRectPixels"] = { {"x", tile.sourceRectPixels.x}, {"y", tile.sourceRectPixels.y}, {"w", tile.sourceRectPixels.w}, {"h", tile.sourceRectPixels.h} };
				tileJson["color"] = { {"r", tile.color.r}, {"g", tile.color.g}, {"b", tile.color.b}, {"a", tile.color.a} };
				tileJson["flipX"] = tile.flipX;
				tileJson["flipY"] = tile.flipY;
				(*current)[key].push_back(tileJson);
			}
			break;
		}
		default:
			break;
		}
	}
	return doc;
}

void buki::Component::Deserialize(const json& doc)
{
	for (const PropertyInfo& propInfo : GetProperties())
	{
		json docCopy = doc;
		json value;
		auto& fields = propInfo.prop.fields;
		char* base = reinterpret_cast<char*>(this);
		auto& lastField = fields[fields.size() - 1];
		void* fieldPtr = base + lastField.offset;
		if (!docCopy.contains(lastField.key))
		{
			for (size_t i = 0; i < fields.size(); ++i)
			{
				const BukiField& field = fields[i];

				if (i != fields.size() - 1)
				{
					if (!docCopy.contains(field.key))
					{
						Log().LogWarning("JSON does not contain key '" + field.key + "' for property '" + field.name + "' in component '" + typeid(*this).name() + " for field: " + lastField.key + "'. Skipping.");
						continue;
					}
					else
					{
						docCopy = docCopy[field.key];
					}
				}
				else
				{
					value = docCopy[field.key];
				}
			}
		}
		else
		{
			value = docCopy[lastField.key];
		}



		switch (propInfo.prop.type)
		{
		case PropertyType::Int:
		case PropertyType::EnumInt:
			if (value.is_number_integer())
				*reinterpret_cast<int*>(fieldPtr) = value.get<int>();
			break;

		case PropertyType::Float:
			if (value.is_number())
				*reinterpret_cast<float*>(fieldPtr) = value.get<float>();
			break;

		case PropertyType::Bool:
			if (value.is_boolean())
				*reinterpret_cast<bool*>(fieldPtr) = value.get<bool>();
			break;

		case PropertyType::String:
		case PropertyType::ImageAsset:
		case PropertyType::AudioAsset:
		case PropertyType::FontAsset:
		case PropertyType::PrefabRef:
			if (value.is_string())
				*reinterpret_cast<std::string*>(fieldPtr) = value.get<std::string>();
			break;

		case PropertyType::StringList:
		case PropertyType::ImageAssetList:
		case PropertyType::AudioAssetList:
		case PropertyType::FontAssetList:
			if (value.is_array())
				*reinterpret_cast<std::vector<std::string>*>(fieldPtr) = value.get<std::vector<std::string>>();
			else
				reinterpret_cast<std::vector<std::string>*>(fieldPtr)->clear();
			break;

		case PropertyType::EntityRef:
			if (value.is_object())
			{
				EntityRef& ref = *reinterpret_cast<EntityRef*>(fieldPtr);
				ref.entityName = value.value("entityName", "");
				ref.prefabPath = value.value("prefabPath", "");
				ref.cached = nullptr;
			}
			break;

		case PropertyType::Vector2:
			if (value.is_object())
			{
				Vector2& v = *reinterpret_cast<Vector2*>(fieldPtr);
				v.x = value.value("x", 0.0f);
				v.y = value.value("y", 0.0f);
			}
			break;

		case PropertyType::Color:
			if (value.is_object())
			{
				Color& c = *reinterpret_cast<Color*>(fieldPtr);
				c.r = value.value("r", 1.0f);
				c.g = value.value("g", 1.0f);
				c.b = value.value("b", 1.0f);
				c.a = value.value("a", 1.0f);
			}
			break;

		case PropertyType::RectF:
			if (value.is_object())
			{
				RectF& r = *reinterpret_cast<RectF*>(fieldPtr);
				r.x = value.value("x", 0.0f);
				r.y = value.value("y", 0.0f);
				r.w = value.value("w", 0.0f);
				r.h = value.value("h", 0.0f);
			}
			break;
		case PropertyType::EnumScriptBinding:
			if (value.is_object())
			{
				StaticFunctionBinding& b = *reinterpret_cast<StaticFunctionBinding*>(fieldPtr);
				b.scriptTypeName = value.value("scriptTypeName", "");
				b.functionName = value.value("functionName", "");
			}
			break;
		case PropertyType::TileList:
		{
			if (value.is_array())
			{
				std::vector<TileDrawData>& tileList = *reinterpret_cast<std::vector<TileDrawData>*>(fieldPtr);
				tileList.clear();
				for (auto& jsonTile : value)
				{
					TileDrawData tile;
					tile.localPosition.x = jsonTile["localPosition"].value("x", 0.0f);
					tile.localPosition.y = jsonTile["localPosition"].value("y", 0.0f);
					tile.size.x = jsonTile["size"].value("x", 1.0f);
					tile.size.y = jsonTile["size"].value("y", 1.0f);
					tile.sourceRectPixels.x = jsonTile["sourceRectPixels"].value("x", 0.0f);
					tile.sourceRectPixels.y = jsonTile["sourceRectPixels"].value("y", 0.0f);
					tile.sourceRectPixels.w = jsonTile["sourceRectPixels"].value("w", 0.0f);
					tile.sourceRectPixels.h = jsonTile["sourceRectPixels"].value("h", 0.0f);
					tile.color.r = jsonTile["color"].value("r", 1.0f);
					tile.color.g = jsonTile["color"].value("g", 1.0f);
					tile.color.b = jsonTile["color"].value("b", 1.0f);
					tile.color.a = jsonTile["color"].value("a", 1.0f);
					tile.flipX = jsonTile.value("flipX", false);
					tile.flipY = jsonTile.value("flipY", false);
					tile.texturePath = dynamic_cast<TileLayer*>(this)->GetTexturePath();
					tileList.push_back(tile);
				}
			}
			break;
		}
		default:
			break;
		}
	}
}

buki::IInput& buki::Component::Input() const
{
	return Engine::Get().Input();
}

buki::ILogger& buki::Component::Log() const
{
	return Engine::Get().Log();
}

buki::IGraphics& buki::Component::Graphics() const
{
	return Engine::Get().Graphics();
}

buki::IAudio& buki::Component::Audio() const
{
	return Engine::Get().Audio();
}

buki::IWorld& buki::Component::World() const
{
	return Engine::Get().World();
}

buki::PhysicsService& buki::Component::Physics() const
{
	return Engine::Get().Physics();
}

buki::TextureManager& buki::Component::Textures() const
{
	return Engine::Get().Textures();
}

buki::FontManager& buki::Component::Fonts() const
{
	return Engine::Get().Fonts();
}

buki::Camera2D& buki::Component::Camera() const
{
	return Engine::Get().GetActiveCamera();
}
