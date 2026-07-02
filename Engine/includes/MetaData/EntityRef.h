#pragma once
#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
 
namespace buki
{
	class Entity;
	struct PrefabAssetInstance;
	struct EntityRef
	{
		std::string entityName = "";
		std::string prefabPath = "";
		Entity* cached = nullptr;

		void Clear();

		bool Empty() const;
	};

	void to_json(json& j, const EntityRef& ref);

	void from_json(const json& j, EntityRef& ref);

	Entity* ResolveEntityRef(EntityRef& ref);

	Entity* ResolvePrefabRef(PrefabAssetInstance& ref);
}