#pragma once
#include "EntityRef.h"
#include "Entity.h"
#include "IWorld.h"
#include "Engine.h"

void buki::EntityRef::Clear()
{
	entityName.clear();
	prefabPath.clear();
	cached = nullptr;
}

bool buki::EntityRef::Empty() const
{
	return entityName.empty() && prefabPath.empty() && cached == nullptr;
}

void buki::to_json(nlohmann::json& j, const EntityRef& ref)
{
	j = nlohmann::json{
			{ "entity", ref.entityName },
			{ "prefab", ref.prefabPath }
	};
}

void buki::from_json(const nlohmann::json& j, EntityRef& ref)
{
	ref.entityName = j.value("entity", "");
	ref.prefabPath = j.value("prefab", "");
	ref.cached = nullptr;
}

buki::Entity* buki::ResolveEntityRef(EntityRef& ref)
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

	//if (!ref.prefabPath.empty())
	//{
	//	//ref.cached = world.InstantiatePrefab(ref.prefabPath);
	//	if (ref.cached != nullptr && ref.entityName.empty())
	//	{
	//	}
	//}

	return ref.cached;
}

buki::Entity* buki::ResolvePrefabRef(PrefabAssetInstance& ref)
{
	if (ref.entity != nullptr)
		return ref.entity;

	//if (!ref.path.empty())
	//{
	//	//ref.entity = Engine::Get().World().InstantiatePrefab(ref.path);
	//	if (ref.entity != nullptr)
	//	{
	//		if (ref.entity->GetName().empty())
	//		{

	//		}
	//	}
	//}

	return ref.entity;
}
