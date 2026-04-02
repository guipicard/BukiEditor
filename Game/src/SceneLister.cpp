#pragma once
#include "SceneLister.h"
#include "Engine.h"
#include "Button.h"
#include "ComponentRegistration.h"

REGISTER_COMPONENT(SceneLister, "SceneLister");
namespace fs = std::filesystem;

buki::SceneLister::SceneLister(buki::Entity* entity)
	: MonoBehaviour(entity)
{
}

void buki::SceneLister::Awake()
{
}

void buki::SceneLister::Start()
{
	Vector2 buttonSize = Vector2(1.0f, 1.0f);
	std::vector<Entity*> scenesEntities = std::vector<Entity*>();
	for (const auto& file : fs::directory_iterator(scenePath))
	{
		if (file.path().extension() == ".json")
		{
			std::string fileName = file.path().stem().string();
			if (fileName == "SceneEditor") continue;
			Entity* buttonEntity = World().Create(fileName + "Button");
			scenesEntities.push_back(buttonEntity);
			buttonEntity->GetTransform()->SetSize(buttonSize);
			Button* button = buttonEntity->AddComponent<Button>();
			button->SetButtonText(fileName, 24);
			button->SetMessage("./Scenes/" + fileName + ".json");
			button->OnClick.AddListener(dynamic_cast<Observer<std::string>*>(buki::Engine::GetInstance().World().GetCurrentScene()));
		}
	}
	int length = scenesEntities.size();
	float screenSizeX, screenSizeY;
	Graphics().GetWindowSize(&screenSizeX, &screenSizeY);
	int i = 1;
	float widthStep = screenSizeX / (length + 1);
	for (auto buttonEntity : scenesEntities)
	{
		buttonEntity->GetTransform()->SetPosition({ (widthStep * i) - ((screenSizeX / 2)),0.0f });
		buttonEntity->GetComponent<Button>()->Set();
		i++;
	}
}

void buki::SceneLister::Update(float dt)
{
}

void buki::SceneLister::FixedUpdate(float dt)
{
}

void buki::SceneLister::Destroy()
{
}

void buki::SceneLister::OnCollisionEnter(Entity* other)
{
}

void buki::SceneLister::OnCollisionExit(Entity* other)
{
}

void buki::SceneLister::OnCollisionHit(Entity* other)
{
}

json buki::SceneLister::Serialize()
{
	return json();
}

void buki::SceneLister::Deserialize(json _doc)
{
}

void buki::SceneLister::Set()
{
}
