#pragma once

#include <functional>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace buki
{
	class Entity;
	class Component;

	class ComponentFactory
	{
	public:
		using CreateFn = std::function<Component* (Entity*, const json&)>;

		static void Register(const std::string& typeName, CreateFn fn, const std::type_info& type);
		static Component* CreateCmp(Entity* e, const std::string& typeName, const json& j);

		static std::string GetTypeName(const std::type_info& type);

		static std::string GetTypeName(const std::type_index type);

		static std::type_index GetTypeId(const std::string name);

		static bool IsRegistered(const std::string& typeName);

		static std::vector<std::string> GetRegisteredTypeNames();

		static std::vector<std::type_index> GetRegisteredTypes();
	};
}