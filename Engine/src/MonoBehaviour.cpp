#include "MonoBehaviour.h"
#include "EntityRef.h"
#include <string>


using json = nlohmann::json;

buki::MonoBehaviour::MonoBehaviour(Entity* entity)
	: Component(entity)
{
}



void buki::MonoBehaviour::Set()
{
	for (const PropertyInfo& propInfo : GetProperties())
	{
		char* base = reinterpret_cast<char*>(this);
		void* fieldPtr = base + propInfo.prop.fields[propInfo.prop.fields.size()-1].offset;

		switch (propInfo.prop.type)
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