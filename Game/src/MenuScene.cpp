#pragma once

#include "MenuScene.h"
#include "Engine.h"
#include "IWorld.h"
#include "IGraphics.h"
#include "IInput.h"
#include "Text.h"
#include "BukiContainers.h"
#include "Button.h"
#include "AngryBirdController.h"
#include "Box.h"

buki::MenuScene::MenuScene()
{
}

void buki::MenuScene::CodeLoad()
{
	Engine::Get().Graphics().SetCameraPosition(0.0f, 0.0f);

	std::string buttonFont = "./fonts/Kenney/Kenney Blocks.ttf";

	float windowW, windowH;
	Engine::Get().GetActiveCamera().GetViewportWorldSize(&windowW, &windowH);

	buki::Entity* TitleEntity = Instantiate("Title");
	Text* TitleText = TitleEntity->AddComponent<Text>();
	TitleText->SetFontPath(buttonFont);
	TitleText->SetFontSize(48);
	TitleText->SetText("Press Enter To Switch Scenes");

	float heightStep = windowH / 8.0f;

	TitleEntity->T()->SetPosition(Vector2(0.0f, -heightStep * 2.5f));

	Vector2 buttonsSize = Vector2(1.0f, 1.0f);

	ButtonStyle style{
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 0.6f, 1.0f },
		{ 0.2f, 0.2f, 0.2f, 1.0f },
		{ 0.3f, 0.3f, 0.3f, 1.0f },
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 0.0f, 1.0f },
		Vector2(0.25f, 0.15f),
		Vector2(0.0f, 0.0f),
		true,
		true,
		false,
		true,
		true,
		true
	};

	buki::Entity* ShapesButtonEntity = Instantiate("ShapesPlayButton");
	ShapesButtonEntity->T()->SetSize(buttonsSize);
	ShapesButtonEntity->T()->SetPosition(Vector2(0.0f, -heightStep * 1.0f));
	Button* playButton = ShapesButtonEntity->AddComponent<Button>();
	playButton->SetStyle(style);
	playButton->SetFontPath(buttonFont);
	playButton->SetFontSize(36);
	playButton->SetText("Shapes Demo");
	playButton->SetMessage("Shapes");

	buki::Entity* AngryBirdButtonEntity = Instantiate("AbPlayButton");
	AngryBirdButtonEntity->T()->SetSize(buttonsSize);
	AngryBirdButtonEntity->T()->SetPosition(Vector2(0.0f, -heightStep * 0.0f));
	Button* AngryBirdButton = AngryBirdButtonEntity->AddComponent<Button>();
	AngryBirdButton->SetStyle(style);
	AngryBirdButton->SetFontPath(buttonFont);
	AngryBirdButton->SetFontSize(36);
	AngryBirdButton->SetText("Angry Bird Demo");
	AngryBirdButton->SetMessage("AngryBird");

	buki::Entity* PPButtonEntity = Instantiate("PlatformerPlayButton");
	PPButtonEntity->T()->SetSize(buttonsSize);
	PPButtonEntity->T()->SetPosition(Vector2(0.0f, heightStep * 1.0f));
	Button* PPButton = PPButtonEntity->AddComponent<Button>();
	PPButton->SetStyle(style);
	PPButton->SetFontPath(buttonFont);
	PPButton->SetFontSize(36);
	PPButton->SetText("Platformer");
	PPButton->SetMessage("Platformer");

	buki::Entity* SceneTestButtonEntity = Instantiate("SceneTestPlayButton");
	SceneTestButtonEntity->T()->SetSize(buttonsSize);
	SceneTestButtonEntity->T()->SetPosition(Vector2(0.0f, heightStep * 2.0f));
	Button* SceneTestButton = SceneTestButtonEntity->AddComponent<Button>();
	SceneTestButton->SetStyle(style);
	SceneTestButton->SetFontPath(buttonFont);
	SceneTestButton->SetFontSize(36);
	SceneTestButton->SetText("SceneTest");
	SceneTestButton->SetMessage("SceneTest");

	buki::Entity* QuitEntity = Instantiate("QuitButton");
	QuitEntity->T()->SetSize(buttonsSize);
	QuitEntity->T()->SetPosition(Vector2(0.0f, heightStep * 3.0f));
	Button* QuitButton = QuitEntity->AddComponent<Button>();
	QuitButton->SetStyle(style);
	QuitButton->SetFontPath(buttonFont);
	QuitButton->SetFontSize(36);
	QuitButton->SetText("Quit");
	QuitButton->SetMessage("Quit");

	SaveScene();
}

void buki::MenuScene::OnWindowResize()
{

}

void buki::MenuScene::OnNotify(const std::string& button)
{
	if (button == "Quit")
	{
		buki::Engine::Get().Platform().RequestQuit();
	}
	buki::Engine::Get().World().SetLoadScene(button);
}
