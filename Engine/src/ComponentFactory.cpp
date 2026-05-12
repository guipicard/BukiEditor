#include "ComponentFactory.h"
#include "Component.h"

#include <unordered_map>
#include <utility>
#include <typeindex>

namespace
{
	std::unordered_map<std::string, buki::ComponentFactory::CreateFn> g_CreateMap;
	std::unordered_map<std::type_index, std::string> g_ReverseMap;
}

void buki::ComponentFactory::Register(const std::string& typeName, CreateFn fn, const std::type_info& type)
{
	g_CreateMap[typeName] = std::move(fn);
	g_ReverseMap[std::type_index(type)] = typeName;
}

buki::Component* buki::ComponentFactory::CreateCmp(Entity* e, const std::string& typeName, const json& j)
{
	auto it = g_CreateMap.find(typeName);
	if (it == g_CreateMap.end())
	{
		return nullptr;
	}

	return it->second(e, j);
}

void buki::ComponentFactory::Destroy()
{
	g_CreateMap.clear();
	g_ReverseMap.clear();
}

std::string buki::ComponentFactory::GetTypeName(const std::type_info& type)
{
	auto it = g_ReverseMap.find(std::type_index(type));
	if (it != g_ReverseMap.end())
	{
		return it->second;
	}

	return "";
}

std::string buki::ComponentFactory::GetTypeName(const std::type_index type)
{
	auto it = g_ReverseMap.find(type);
	if (it != g_ReverseMap.end())
	{
		return it->second;
	}

	return "";
}

std::type_index buki::ComponentFactory::GetTypeId(const std::string name)
{
	for (const auto& pair : g_ReverseMap)
	{
		if (pair.second == name)
		{
			return pair.first;
		}
	}
	return std::type_index(typeid(void));
}

bool buki::ComponentFactory::IsRegistered(const std::string& typeName)
{
	return g_CreateMap.find(typeName) != g_CreateMap.end();
}

std::vector<std::string> buki::ComponentFactory::GetRegisteredTypeNames()
{
	std::vector<std::string> typeNames;
	for (const auto& pair : g_CreateMap)
	{
		typeNames.push_back(pair.first);
	}
	return typeNames;
}

std::vector<std::type_index> buki::ComponentFactory::GetRegisteredTypes()
{

	std::vector<std::type_index> typeIds;
	for (const auto& pair : g_ReverseMap)
	{
		typeIds.push_back(pair.first);
	}
	return typeIds;
}
