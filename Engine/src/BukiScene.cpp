#pragma once
#include "BukiScene.h"
#include "Engine.h"
#include "Entity.h"
#include "Button.h"
#include "Observer.h"
#include "nlohmann/json.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include <unordered_set>

using json = nlohmann::json;

void buki::BukiScene::OnStart()
{
	for (auto entity : buki::Engine::Get().World().GetEntitiesInWorld())
	{
		if (entity != nullptr)
		{
			entity->Start();
		}
	}
}

void buki::BukiScene::OnStop()
{
	for (auto entity : buki::Engine::Get().World().GetEntitiesInWorld())
	{
		if (entity != nullptr)
		{
			//Implementer une méthode Stop() dans Entity si nécessaire pour gérer les actions à effectuer lors de l'arrêt de la scène.
		}
	}
}

bool buki::BukiScene::Load()
{
	if (name.empty())
	{
		return false;
	}

	const std::string path = "../Deployment/Scenes/" + name + ".scene";

	if (std::filesystem::exists(path))
	{
		FileLoad(path);
		SetScene();
	}
	else
	{
		buki::Engine::Get().Log().LogError("Scene not found: " + path);
		return false;
	}
	return true;
}

void buki::BukiScene::Initialize()
{
}

void buki::BukiScene::SaveScene() const
{
	if (name.empty())
	{
		return;
	}

	SaveScene(name);
}

void buki::BukiScene::SaveScene(std::string fileName) const
{
	if (fileName.empty())
	{
		return;
	}

	std::filesystem::create_directories("../Deployment/Scenes");

	json doc;

	if (auto* camera = buki::Engine::Get().GetActiveCameraPtr(); camera != nullptr)
	{
		doc["camera"] = camera->Serialize();
	}

	doc["entities"] = json::object();

	std::unordered_set<std::string> usedNames;
	IWorld& world = buki::Engine::Get().World();

	for (Entity* entity : world.GetEntitiesInWorld())
	{
		if (entity == nullptr)
		{
			continue;
		}

		std::string entityName = entity->GetName();
		if (entityName.empty())
		{
			entityName = "Entity";
		}

		std::string uniqueName = entityName;
		int suffix = 1;
		while (usedNames.count(uniqueName) > 0)
		{
			uniqueName = entityName + "_" + std::to_string(suffix++);
		}
		usedNames.insert(uniqueName);

		doc["entities"][uniqueName] = entity->Serialize();
	}

	const std::string path = "../Deployment/Scenes/" + fileName + ".scene";
	std::ofstream out(path, std::ios::out | std::ios::trunc);
	if (!out.is_open())
	{
		return;
	}

	out << doc.dump(4);
	out.close();
}

void buki::BukiScene::FileLoad(std::string _path) const
{
	std::ifstream in(_path);
	if (!in.is_open())
	{
		return;
	}

	json doc;
	in >> doc;
	in.close();

	if (doc.contains("camera"))
	{
		if (auto* camera = buki::Engine::Get().GetActiveCameraPtr(); camera != nullptr)
		{
			camera->Deserialize(doc["camera"]);
		}
	}

	IWorld& world = buki::Engine::Get().World();

	if (!doc.contains("entities") || !doc["entities"].is_object())
	{
		return;
	}

	const json& entitiesJson = doc["entities"];

	for (auto it = entitiesJson.begin(); it != entitiesJson.end(); ++it)
	{
		Entity* entity = world.CreateEntity(it.key());
		if (entity == nullptr)
		{
			continue;
		}
		entity->Deserialize(it.value());
	}
}

void buki::BukiScene::SetScene()
{
	IWorld& world = buki::Engine::Get().World();
	std::vector<Entity*> entities = world.GetEntitiesInWorld();
	for (auto it = entities.begin(); it != entities.end(); ++it)
	{
		if (*it == nullptr)
		{
			continue;
		}
		(*it)->Set();
	}
}

void buki::BukiScene::OnWindowResize()
{
}
