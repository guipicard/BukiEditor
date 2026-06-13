#include "MonoBehaviour.h"
#include "EntityRef.h"
#include <string>


using json = nlohmann::json;

buki::MonoBehaviour::MonoBehaviour(Entity* entity)
	: Component(entity)
{
}

json buki::MonoBehaviour::Serialize()
{
	json doc = json::object();

	for (const PropertyInfo& prop : GetProperties())
	{
		const char* base = reinterpret_cast<const char*>(this);
		const void* fieldPtr = base + prop.offset;

		switch (prop.type)
		{
		case PropertyType::Int:
			doc[prop.name] = *reinterpret_cast<const int*>(fieldPtr);
			break;

		case PropertyType::Float:
			doc[prop.name] = *reinterpret_cast<const float*>(fieldPtr);
			break;

		case PropertyType::Bool:
			doc[prop.name] = *reinterpret_cast<const bool*>(fieldPtr);
			break;

		case PropertyType::String:
		case PropertyType::ImageAsset:
		case PropertyType::AudioAsset:
		case PropertyType::PrefabRef:
			doc[prop.name] = *reinterpret_cast<const std::string*>(fieldPtr);
			break;

		case PropertyType::StringList:
		case PropertyType::ImageAssetList:
		case PropertyType::AudioAssetList:
			doc[prop.name] = *reinterpret_cast<const std::vector<std::string>*>(fieldPtr);
			break;

		case PropertyType::EntityRef:
		{
			const EntityRef& ref = *reinterpret_cast<const EntityRef*>(fieldPtr);
			doc[prop.name] = {
				{ "entityName", ref.entityName },
				{ "prefabPath", ref.prefabPath }
			};
			break;
		}

		default:
			break;
		}
	}

	return doc;
}

void buki::MonoBehaviour::Deserialize(json doc)
{
	for (const PropertyInfo& prop : GetProperties())
	{
		if (!doc.contains(prop.name))
		{
			continue;
		}

		char* base = reinterpret_cast<char*>(this);
		void* fieldPtr = base + prop.offset;

		switch (prop.type)
		{
		case PropertyType::Int:
			if (doc.contains(prop.name) && doc[prop.name].is_number_integer())
				*reinterpret_cast<int*>(fieldPtr) = doc[prop.name].get<int>();
			break;

		case PropertyType::Float:
			if (doc.contains(prop.name) && doc[prop.name].is_number())
				*reinterpret_cast<float*>(fieldPtr) = doc[prop.name].get<float>();
			break;

		case PropertyType::Bool:
			if (doc.contains(prop.name) && doc[prop.name].is_boolean())
				*reinterpret_cast<bool*>(fieldPtr) = doc[prop.name].get<bool>();
			break;

		case PropertyType::String:
		case PropertyType::ImageAsset:
		case PropertyType::AudioAsset:
		case PropertyType::PrefabRef:
			if (doc.contains(prop.name) && doc[prop.name].is_string())
				*reinterpret_cast<std::string*>(fieldPtr) = doc[prop.name].get<std::string>();
			break;

		case PropertyType::StringList:
		case PropertyType::ImageAssetList:
		case PropertyType::AudioAssetList:
			if (doc.contains(prop.name) && doc[prop.name].is_array())
				*reinterpret_cast<std::vector<std::string>*>(fieldPtr) = doc[prop.name].get<std::vector<std::string>>();
			else
				reinterpret_cast<std::vector<std::string>*>(fieldPtr)->clear();
			break;

		case PropertyType::EntityRef:
			if (doc.contains(prop.name) && doc[prop.name].is_object())
			{
				EntityRef& ref = *reinterpret_cast<EntityRef*>(fieldPtr);
				const auto& refJson = doc[prop.name];

				ref.entityName = refJson.value("entityName", "");
				ref.prefabPath = refJson.value("prefabPath", "");
				ref.cached = nullptr;
			}
			break;

		default:
			break;
		}
	}
}

void buki::MonoBehaviour::Set()
{
	for (const PropertyInfo& prop : GetProperties())
	{
		char* base = reinterpret_cast<char*>(this);
		void* fieldPtr = base + prop.offset;

		switch (prop.type)
		{
		case PropertyType::EntityRef:
			ResolveEntityRef(*reinterpret_cast<EntityRef*>(fieldPtr));
			break;
		case PropertyType::PrefabRef:
			ResolvePrefabRef(*reinterpret_cast<PrefabAssetInstance*>(fieldPtr));
			break;

		default:
			break;
		}
	}

	OnSet();
}