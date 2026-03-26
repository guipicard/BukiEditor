#pragma once

#include "MenuScene.h"
#include "Engine.h"
#include "IWorld.h"
#include "IGraphics.h"
#include "IInput.h"
#include "Text.h"
#include "Vector2.h"
#include "Button.h"
#include "Component.h"
#include "AngryBirdController.h"
#include "Box.h"

buki::MenuScene::MenuScene()
{
}

void buki::MenuScene::CodeLoad()
{
	buki::Engine::GetInstance().Graphics().SetCameraPosition(Vector2::ZERO);
	float windowW, windowH;
	Engine::GetInstance().Graphics().GetWindowSize(&windowW, &windowH);

	buki::Entity* TitleEntity = Instantiate("Title");
	Text* TitleText = TitleEntity->AddComponent<Text>();
	TitleText->LoadText("./fonts/Kenney/Kenney Blocks.ttf", 48);
	TitleText->SetFixed(false);
	TitleText->SetText("Press Enter To Switch Scenes");
	float heightStep = windowH / 5;
	TitleEntity->GetTransform()->SetPosition(Vector2(0.0f, -heightStep * 2.0f));

	Vector2 buttonsSize = Vector2(1.0f, 1.0f);

	buki::Entity* ShapesButtonEntity = Instantiate("ShapesPlayButton");
	ShapesButtonEntity->GetTransform()->SetSize(buttonsSize);
	ShapesButtonEntity->GetTransform()->SetPosition(Vector2(0.0f, -heightStep));
	Button* playButton = ShapesButtonEntity->AddComponent<Button>();
	playButton->SetMessage("Shapes");
	playButton->SetbuttonFont("./fonts/Kenney/Kenney Blocks.ttf");
	playButton->SetButtonText("Shapes Demo", 36);
	playButton->OnClick.AddListener(this);
	playButton->Set();

	buki::Entity* AngryBirdButtonEntity = Instantiate("AbPlayButton");
	AngryBirdButtonEntity->GetTransform()->SetSize(buttonsSize);
	AngryBirdButtonEntity->GetTransform()->SetPosition(Vector2(0.0f, 0.0f));
	Button* AngryBirdButton = AngryBirdButtonEntity->AddComponent<Button>();
	AngryBirdButton->SetMessage("AngryBird");
	AngryBirdButton->SetbuttonFont("./fonts/Kenney/Kenney Blocks.ttf");
	AngryBirdButton->SetButtonText("Angry Bird Demo", 36);
	AngryBirdButton->OnClick.AddListener(this);
	AngryBirdButton->Set();


	buki::Entity* PPButtonEntity = Instantiate("PlatformerPlayButton");
	PPButtonEntity->GetTransform()->SetSize(buttonsSize);
	PPButtonEntity->GetTransform()->SetPosition(Vector2(0.0f, heightStep * 1.0f));
	Button* PPButton = PPButtonEntity->AddComponent<Button>();
	PPButton->SetMessage("Platformer");
	PPButton->SetbuttonFont("./fonts/Kenney/Kenney Blocks.ttf");
	PPButton->SetButtonText("Platformer", 36);
	PPButton->OnClick.AddListener(this);
	PPButton->Set();

	buki::Entity* QuitEntity = Instantiate("QuitButton");
	QuitEntity->GetTransform()->SetSize(buttonsSize);
	QuitEntity->GetTransform()->SetPosition(Vector2(0.0f, heightStep * 2.0f));
	Button* QuitButton = QuitEntity->AddComponent<Button>();
	QuitButton->SetMessage("Quit");
	QuitButton->SetbuttonFont("./fonts/Kenney/Kenney Blocks.ttf");
	QuitButton->SetButtonText("Quit", 36);
	QuitButton->OnClick.AddListener(this);
	QuitButton->Set();

	SaveScene();
}

void buki::MenuScene::OnWindowResize()
{

}

void buki::MenuScene::OnNotify(const std::string& button)
{
	if (button == "Quit") buki::Engine::GetInstance().Input().ExitProgram();
	buki::Engine::GetInstance().World().SetLoadScene(button);
}
