#pragma once
#include "engine.h"
#include "SceneEditor.h"
#include "Sprite.h"
#include "Entity.h"
#include "Button.h"
#include "SceneLister.h"
#include "Camera.h"
#include "EditorController.h"
#include "EditorEntity.h"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>

using json = nlohmann::json;

buki::SceneEditor::SceneEditor()
{
}

void buki::SceneEditor::CodeLoad()
{
	buki::Engine::GetInstance().Graphics().SetCameraPosition(Vector2::ZERO);
	Entity* c = Instantiate("EditorController");
	c->AddComponent<SceneLister>();
	//SaveScene();
}

void buki::SceneEditor::OnWindowResize()
{
}

void buki::SceneEditor::OnNotify(const std::string& button)
{
	buki::Engine::GetInstance().World().Unload();
	EditorLoadScene(button);
}

void buki::SceneEditor::OnStop()
{
}

void buki::SceneEditor::EditorLoadScene(std::string scene)
{
	std::ifstream in(scene);
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
		Transform* t = entity->GetTransform();
		t->SetPosition(Vector2(it.value()["position"]["x"].get<float>(), it.value()["position"]["y"].get<float>()));
		t->SetSize(Vector2(it.value()["size"]["x"].get<float>(), it.value()["size"]["y"].get<float>()));
		t->SetRotation(it.value()["rotation"].get<float>());
		entity->SetZ(it.value()["z"].get<int>());
		entity->SetLayer(it.value()["layer"].get<std::string>());
		EditorEntity* e = entity->AddComponent<EditorEntity>();
		e->SetScenePath(scene);
		for (auto& componentData : it.value()["components"].items())
		{
			const std::string& typeName = componentData.key();
			json cmpData = componentData.value();
			e->AddComponentData(typeName, cmpData);
		}
		e->Start();
	}
	Entity* c = Instantiate("EditorController");
	c->AddComponent<EditorController>();
}

void buki::SceneEditor::TestImgui()
{
	
}
