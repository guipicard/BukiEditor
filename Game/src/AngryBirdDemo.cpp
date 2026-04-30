#include "AngryBirdDemo.h"
#include "Engine.h"
#include "Entity.h"
#include "Sprite.h"
#include "TileLayer.h"
#include "RigidBody.h"
#include "Box.h"
#include "Spawner.h"
#include "Button.h"
#include "AngryBirdController.h"
#include "Prefabs.h"


buki::AngryBirdDemo::AngryBirdDemo()
{
	spawner = nullptr;
}

void buki::AngryBirdDemo::CodeLoad()
{
	spawner = Instantiate("spawner")->AddComponent<Spawner>();
	buki::Engine::Get().Graphics().SetCameraPosition(0.0f,0.0f);

	spawner->AddPrototype("ButtonSimple", new ButtonSimple());

	CreateMenu();

	SaveScene();
}

void buki::AngryBirdDemo::OnWindowResize()
{
	//GroundAndBackground();
}

void buki::AngryBirdDemo::OnNotify(const std::string& button)
{
	if (button == "Ag_Lvl_1")
	{
		buki::Engine::Get().World().SetLoadScene("Ag_Lvl_1");
	}
	if (button == "Ag_Lvl_2")
	{
		buki::Engine::Get().World().SetLoadScene("Ag_Lvl_2");
	}
	if (button == "Ag_Lvl_3")
	{
		buki::Engine::Get().World().SetLoadScene("Ag_Lvl_3");
	}
	if (button == "menu")
	{
		buki::Engine::Get().World().SetLoadScene("Menu");
	}
}

void buki::AngryBirdDemo::CreateMenu()
{
	Vector2 buttonsSize = Vector2(5.0f, 2.0f);

	

	buki::Entity* lvl1ButtonEntity = spawner->Spawn("ButtonSimple", Vector2(0.0f, -7.5f), buttonsSize, 0.0f);
	Button* playlvl1Button = lvl1ButtonEntity->GetComponent<Button>();
	playlvl1Button->SetText("Level 1");
	playlvl1Button->SetMessage("Ag_Lvl_1");

	buki::Entity* lvl2ButtonEntity = spawner->Spawn("ButtonSimple", Vector2(0.0f, -2.5f), buttonsSize, 0.0f);
	Button* playlvl2Button = lvl2ButtonEntity->GetComponent<Button>();
	playlvl2Button->SetText("Level 2");
	playlvl2Button->SetMessage("Ag_Lvl_2");

	buki::Entity* lvl3ButtonEntity = spawner->Spawn("ButtonSimple", Vector2(0.0f, 2.5f), buttonsSize, 0.0f);
	Button* playlvl3Button = lvl3ButtonEntity->GetComponent<Button>();
	playlvl3Button->SetText("Level 3");
	playlvl3Button->SetMessage("Ag_Lvl_3");

	buki::Entity* menuButtonEntity = spawner->Spawn("ButtonSimple", Vector2(0.0f, 7.5f), buttonsSize, 0.0f);
	Button* menuButtonButton = menuButtonEntity->GetComponent<Button>();
	menuButtonButton->SetText("menu");
	menuButtonButton->SetMessage("menu");
}

