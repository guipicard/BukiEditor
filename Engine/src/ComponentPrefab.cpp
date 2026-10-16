#pragma once
#include "ComponentPrefab.h"

bool buki::ComponentPrefab::LoadFromFile(const std::string& path)
{
	return false;
}

bool buki::ComponentPrefab::SaveToFile(const std::string& path) const
{
	return false;
}

bool buki::ComponentPrefab::LoadIntoComponent(Component* component) const
{
	return false;
}

bool buki::ComponentPrefab::SaveFromComponent(const Component* component)
{
	return false;
}
