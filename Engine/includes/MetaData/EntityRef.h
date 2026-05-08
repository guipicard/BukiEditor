#pragma once
#include <string>
#include "nlohmann/json.hpp"
#include "Engine.h"
#include "Entity.h"

namespace buki
{

	struct EntityRef
	{
		std::string entityName;
		std::string prefabPath;
		Entity* cached = nullptr;

		void Clear()
		{
			entityName.clear();
			prefabPath.clear();
			cached = nullptr;
		}

		bool Empty() const
		{
			return entityName.empty() && prefabPath.empty() && cached == nullptr;
		}
	};

	inline void to_json(nlohmann::json& j, const EntityRef& ref)
	{
		j = nlohmann::json{
			{ "entity", ref.entityName },
			{ "prefab", ref.prefabPath }
		};
	}

	inline void from_json(const nlohmann::json& j, EntityRef& ref)
	{
		ref.entityName = j.value("entity", "");
		ref.prefabPath = j.value("prefab", "");
		ref.cached = nullptr;
	}


	inline Entity* ResolveEntityRef(EntityRef& ref)
	{
		if (ref.cached != nullptr)
			return ref.cached;

		IWorld& world = Engine::Get().World();

		if (!ref.entityName.empty())
		{
			ref.cached = world.FindEntityByName(ref.entityName);
			if (ref.cached != nullptr)
				return ref.cached;
		}

		if (!ref.prefabPath.empty())
		{
			//ref.cached = world.InstantiatePrefab(ref.prefabPath);
			if (ref.cached != nullptr && ref.entityName.empty())
			{
				
			}
		}

		return ref.cached;
	}
}