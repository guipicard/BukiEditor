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
			doc[prop.name] = *reinterpret_cast<const std::string*>(fieldPtr);
			break;

		case PropertyType::StringList:
			doc[prop.name] = *reinterpret_cast<const std::vector<std::string>*>(fieldPtr);
			break;

		case PropertyType::ImageAsset:
			doc[prop.name] = *reinterpret_cast<const std::string*>(fieldPtr);
			break;

		case PropertyType::AudioAsset:
			doc[prop.name] = *reinterpret_cast<const std::string*>(fieldPtr);
			break;

		case PropertyType::EntityRef:
			doc[prop.name] = *reinterpret_cast<const EntityRef*>(fieldPtr);
			break;

		case PropertyType::PrefabRef:
			doc[prop.name] = *reinterpret_cast<const std::string*>(fieldPtr);
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
			if (doc[prop.name].is_number_integer())
			{
				*reinterpret_cast<int*>(fieldPtr) = doc[prop.name].get<int>();
			}
			break;

		case PropertyType::Float:
			if (doc[prop.name].is_number())
			{
				*reinterpret_cast<float*>(fieldPtr) = doc[prop.name].get<float>();
			}
			break;

		case PropertyType::Bool:
			if (doc[prop.name].is_boolean())
			{
				*reinterpret_cast<bool*>(fieldPtr) = doc[prop.name].get<bool>();
			}
			break;

		case PropertyType::String:
			if (doc[prop.name].is_string())
			{
				*reinterpret_cast<std::string*>(fieldPtr) = doc[prop.name].get<std::string>();
			}
			break;

		case PropertyType::StringList:
			if (doc[prop.name].is_array())
			{
				*reinterpret_cast<std::vector<std::string>*>(fieldPtr) =
					doc[prop.name].get<std::vector<std::string>>();
			}
			break;

		case PropertyType::ImageAsset:
			if (doc[prop.name].is_string())
			{
				*reinterpret_cast<std::string*>(fieldPtr) = doc[prop.name].get<std::string>();
			}
			break;

		case PropertyType::AudioAsset:
			if (doc[prop.name].is_string())
			{
				*reinterpret_cast<std::string*>(fieldPtr) = doc[prop.name].get<std::string>();
			}
			break;

		case PropertyType::EntityRef:
			if (doc[prop.name].is_object())
			{
				*reinterpret_cast<EntityRef*>(fieldPtr) = doc[prop.name].get<EntityRef>();
			}
			break;

		case PropertyType::PrefabRef:
			if (doc[prop.name].is_string())
			{
				*reinterpret_cast<std::string*>(fieldPtr) = doc[prop.name].get<std::string>();
			}
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

		default:
			break;
		}
	}

	OnSet();
}