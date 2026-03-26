#pragma once
#include "Engine.h"
#include "BukiScene.h"
#include "Entity.h"
#include "Camera.h"
#include "Button.h"
#include "Observer.h"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>

using json = nlohmann::json;

buki::BukiScene::BukiScene()
{

}

buki::BukiScene::~BukiScene()
{
}

buki::Entity* buki::BukiScene::Instantiate(const std::string _name)
{
	return buki::Engine::GetInstance().World().Create(_name);
}

void buki::BukiScene::OnStart()
{
	for (auto entity : buki::Engine::GetInstance().World().GetEntitiesInWorld())
	{
		entity->Start();
	}
}

void buki::BukiScene::OnStop()
{
}

void buki::BukiScene::Load()
{
	if (name == "")
	{
		//buki::Engine::GetInstance().Log().LogMessage("Scene name is empty, cannot load scene.");
		return;
	}
	Engine::GetInstance().Graphics().ResetScale();
	std::string path = "./Scenes/" + name + ".json";
	if (std::filesystem::exists(path))
	{
		//buki::Engine::GetInstance().Log().LogMessage("Scene file found, loading scene: " + name);
		FileLoad(path);
	}
	else
	{
		//buki::Engine::GetInstance().Log().LogMessage("Scene file not found, loading scene: " + name + " with code.");
		CodeLoad();
		SaveScene();
	}
}

void buki::BukiScene::Initialize()
{
}

void buki::BukiScene::SaveScene() const 
{
	if (name == "") return;
	std::string path = "./Scenes/" + name + ".json";
	if (!std::filesystem::exists(path))
	{
		json doc;
		doc["sceneName"] = name;
		Camera* camera = buki::Engine::GetInstance().Graphics().GetCamera();
		json cameraDoc = camera->Serialize();
		doc["camera"] = cameraDoc;

		json entitiesJson;
		IWorld& world = buki::Engine::GetInstance().World();
		for (auto entity : world.GetEntitiesInWorld())
		{
			entitiesJson[entity->GetName()] = (entity->Serialize());
			buki::Engine::GetInstance().Log().LogMessage(entity->GetName());
		}
		doc["entities"] = entitiesJson;
		std::ofstream out(path);
		out << doc.dump(4);
		out.close();
	}
}

void buki::BukiScene::SaveScene(std::string fileName) const
{
	if (fileName == "") return;
	std::string path = "./Scenes/" + fileName + ".json";
	if (!std::filesystem::exists(path))
	{
		json doc;
		doc["sceneName"] = fileName;
		Camera* camera = buki::Engine::GetInstance().Graphics().GetCamera();
		json cameraDoc = camera->Serialize();
		doc["camera"] = cameraDoc;

		json entitiesJson;
		IWorld& world = buki::Engine::GetInstance().World();
		for (auto entity : world.GetEntitiesInWorld())
		{
			entitiesJson[entity->GetName()] = (entity->Serialize());
			//buki::Engine::GetInstance().Log().LogMessage(entity->GetName());
		}
		doc["entities"] = entitiesJson;
		std::ofstream out(path);
		out << doc.dump(4);
		out.close();
	}
}

void buki::BukiScene::FileLoad(std::string _path) const 
{
	std::ifstream in(_path);
	json doc;
	in >> doc;
	in.close();
	Camera* camera = buki::Engine::GetInstance().Graphics().GetCamera();
	json cameraDoc = doc["camera"];
	camera->Deserialize(cameraDoc);
	IWorld& world = buki::Engine::GetInstance().World();
	json entitiesJson = doc["entities"];
	//buki::Engine::GetInstance().Log().LogMessage("Loading from file Scene: " + name);
	for (auto it = entitiesJson.begin(); it != entitiesJson.end(); ++it)
	{
		//buki::Engine::GetInstance().Log().LogMessage(it.key());
		Entity* entity = world.Create(it.key());
		entity->Deserialize(it.value());

		Button* eButton = entity->GetComponent<Button>();
		if (eButton)
		{
			eButton->OnClick.AddListener(dynamic_cast<Observer<std::string>*>(buki::Engine::GetInstance().World().GetCurrentScene()));
		}
	}
	//buki::Engine::GetInstance().Log().LogMessage("Scene " + name + " loaded.");
}
