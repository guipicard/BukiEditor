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

using json = nlohmann::json;

buki::BukiScene::BukiScene()
{
}

buki::BukiScene::~BukiScene()
{
}

buki::Entity* buki::BukiScene::Instantiate(const std::string _name)
{
	return buki::Engine::Get().World().Create(_name);
}

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
}

void buki::BukiScene::Load()
{
	if (name.empty())
	{
		return;
	}

	const std::string path = "../Deployment/Scenes/" + name + ".json";

	if (std::filesystem::exists(path))
	{
		FileLoad(path);
		SetScene();
		buki::Engine::Get().Log().LogMessage("Scene loaded: " + name);
	}
	else
	{
		CodeLoad();
		SetScene();
		SaveScene();
		buki::Engine::Get().Log().LogMessage("Scene saved: " + name);
	}
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

	const std::string path = "../Deployment/Scenes/" + fileName + ".json";
	std::filesystem::create_directories("../Deployment/Scenes");

	json doc;
	doc["sceneName"] = fileName;

	if (auto* camera = buki::Engine::Get().GetActiveCameraPtr(); camera != nullptr)
	{
		doc["camera"] = camera->Serialize();
	}

	json entitiesJson = json::object();
	IWorld& world = buki::Engine::Get().World();

	for (auto entity : world.GetEntitiesInWorld())
	{
		if (entity != nullptr)
		{
			entitiesJson[entity->GetName()] = entity->Serialize();
		}
	}

	doc["entities"] = entitiesJson;

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
		Entity* entity = world.Create(it.key());
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
