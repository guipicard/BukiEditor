#pragma once
#include "Prefabs.h"
#include "Engine.h"
#include "Component.h"
#include "Sprite.h"
#include "Circle.h"
#include "Box.h"
#include "RigidBody.h"
#include "Button.h"
#include "Text.h"
#include "AngryBirdController.h"
#include "BricksBehaviour.h"

buki::Entity* buki::CircleShape::Clone(Vector2 position, Vector2 size, float rotation)
{
	number++;
	std::string name = "circle" + std::to_string(number);
	Entity* circleEntity = Engine::GetInstance().World().Create(name);
	int radius = rand() % 6 +2;
	float r = (float)radius;
	r /= 4;
	float rot = static_cast<float>((rand() % 7000) / 1000.0f);
	Vector2 randomSize = Vector2(r, r);
	circleEntity->Initialize(position, 0.0f, randomSize);

	Circle* col = circleEntity->AddComponent<Circle>();
	col->Collider.Density = 1.0f;
	col->Collider.Friction = 0.3f;
	//col->Collider.RestitutionThreshold = 0.9f;
	col->Collider.Radius = r /2;
	col->ShapeColor = Color::LIGHTCORAL;
	col->DebugColor = Color::RED;
	col->shapeDraw = true;
	col->fillDraw = true;

	circleEntity->ActivatePhysics();
	return circleEntity;
}

buki::Entity* buki::RectangleShape::Clone(Vector2 position, Vector2 size, float rotation)
{
	number++;
	std::string name = "rect" + std::to_string(number);
	Entity* rectEntity = Engine::GetInstance().World().Create(name);
	int sizeX = rand() % 10 + 2;
	int sizeY = rand() % 10 + 2;
	float rot = static_cast<float>((rand() % 7000) / 1000.0f);
	Vector2 randomSize = Vector2(sizeX, sizeY) / 5;
	rectEntity->Initialize(position, rot, randomSize);
	rectEntity->SetZ(1);

	Box* col = rectEntity->AddComponent<Box>();
	col->Collider.Density = 1.0f;
	col->Collider.Friction = 0.3f;
	//col->Collider.RestitutionThreshold = 0.9f;
	col->Collider.Size = randomSize / 2;
	col->ShapeColor = Color::LIGHTGREEN;
	col->DebugColor = Color::GREEN;
	col->shapeDraw = true;
	col->fillDraw = true;

	rectEntity->ActivatePhysics();

	return rectEntity;
}

buki::Entity* buki::StoneSlim::Clone(Vector2 position, Vector2 size, float rotation)
{
	number++;
	std::string name = "StoneSlim" + std::to_string(number);
	Entity* rectEntity = Engine::GetInstance().World().Create(name);
	rectEntity->Initialize(position, rotation, size);

	rectEntity->AddComponent<Sprite>();

	Box* col = rectEntity->AddComponent<Box>();
	col->Collider.Density = 5.0f;
	col->Collider.Friction = 0.8f;
	col->Collider.Restitution = 0.3f;
	col->Collider.Size = size / 2;
	
	rectEntity->ActivatePhysics();

	BricksBehaviour* bh = rectEntity->AddComponent<BricksBehaviour>();
	bh->SetMaxHealth(100.0f);

	bh->AddImage("./assets/Kenney/Physics Assets/PNG/Stone elements/elementStone020.png");
	bh->AddImage("./assets/Kenney/Physics Assets/PNG/Stone elements/elementStone025.png");
	bh->AddImage("./assets/Kenney/Physics Assets/PNG/Stone elements/elementStone052.png");

	bh->AddCollisionSound("./audio/AngryBird/Sfx - Rock Collision A1.mp3");
	bh->AddCollisionSound("./audio/AngryBird/Sfx - Rock Collision A2.mp3");
	bh->AddCollisionSound("./audio/AngryBird/Sfx - Rock Collision A3.mp3");
	bh->AddCollisionSound("./audio/AngryBird/Sfx - Rock Collision A4.mp3");
	bh->AddCollisionSound("./audio/AngryBird/Sfx - Rock Collision A5.mp3");

	bh->AddDamageSound("./audio/AngryBird/Sfx - Rock Damage A1.mp3");
	bh->AddDamageSound("./audio/AngryBird/Sfx - Rock Damage A2.mp3");
	bh->AddDamageSound("./audio/AngryBird/Sfx - Rock Damage A3.mp3");

	bh->AddBreakSound("./audio/AngryBird/Sfx - Rock Destroyed A1.mp3");
	bh->AddBreakSound("./audio/AngryBird/Sfx - Rock Destroyed A2.mp3");
	bh->AddBreakSound("./audio/AngryBird/Sfx - Rock Destroyed A3.mp3");

	return rectEntity;
}

buki::Entity* buki::WoodSlim::Clone(Vector2 position, Vector2 size, float rotation)
{
	number++;
	std::string name = "WoodSlim" + std::to_string(number);
	Entity* rectEntity = Engine::GetInstance().World().Create(name);
	rectEntity->Initialize(position, rotation, size);

	Sprite* m_Sprite = rectEntity->AddComponent<Sprite>();

	Box* col = rectEntity->AddComponent<Box>();
	col->Collider.Density = 3.0f;
	col->Collider.Friction = 0.4f;
	col->Collider.Restitution = 0.3f;
	col->Collider.Size = size / 2;

	rectEntity->ActivatePhysics();

	BricksBehaviour* bh = rectEntity->AddComponent<BricksBehaviour>();
	bh->SetMaxHealth(75.0f);

	bh->AddImage("./assets/Kenney/Physics Assets/PNG/Wood elements/elementWood019.png");
	bh->AddImage("./assets/Kenney/Physics Assets/PNG/Wood elements/elementWood024.png");
	bh->AddImage("./assets/Kenney/Physics Assets/PNG/Wood elements/elementWood051.png");

	bh->AddCollisionSound("./audio/AngryBird/Sfx - Wood Collision A1.mp3");
	bh->AddCollisionSound("./audio/AngryBird/Sfx - Wood Collision A2.mp3");
	bh->AddCollisionSound("./audio/AngryBird/Sfx - Wood Collision A3.mp3");
	bh->AddCollisionSound("./audio/AngryBird/Sfx - Wood Collision A4.mp3");
	bh->AddCollisionSound("./audio/AngryBird/Sfx - Wood Collision A5.mp3");
	bh->AddCollisionSound("./audio/AngryBird/Sfx - Wood Collision A6.mp3");

	bh->AddDamageSound("./audio/AngryBird/Sfx - Wood Damage A1.mp3");
	bh->AddDamageSound("./audio/AngryBird/Sfx - Wood Damage A2.mp3");
	bh->AddDamageSound("./audio/AngryBird/Sfx - Wood Damage A3.mp3");

	bh->AddBreakSound("./audio/AngryBird/Sfx - Wood Destroyed A1.mp3");
	bh->AddBreakSound("./audio/AngryBird/Sfx - Wood Destroyed A2.mp3");
	bh->AddBreakSound("./audio/AngryBird/Sfx - Wood Destroyed A3.mp3");

	return rectEntity;
}

buki::Entity* buki::GlassSlim::Clone(Vector2 position, Vector2 size, float rotation)
{
	number++;
	std::string name = "GlassSlim" + std::to_string(number);
	Entity* rectEntity = Engine::GetInstance().World().Create(name);
	rectEntity->Initialize(position, rotation, size);

	Sprite* m_Sprite = rectEntity->AddComponent<Sprite>();

	Box* col = rectEntity->AddComponent<Box>();
	col->Collider.Density = 1.0f;
	col->Collider.Friction = 0.1f;
	col->Collider.Restitution = 0.3f;
	col->Collider.Size = size / 2;

	rectEntity->ActivatePhysics();

	BricksBehaviour* bh = rectEntity->AddComponent<BricksBehaviour>();
	bh->SetMaxHealth(50.0f);

	bh->AddImage("./assets/Kenney/Physics Assets/PNG/Glass elements/elementGlass021.png");
	bh->AddImage("./assets/Kenney/Physics Assets/PNG/Glass elements/elementGlass026.png");
	bh->AddImage("./assets/Kenney/Physics Assets/PNG/Glass elements/elementGlass053.png");

	bh->AddCollisionSound("./audio/AngryBird/Sfx - Ice Light Collision A1.mp3");
	bh->AddCollisionSound("./audio/AngryBird/Sfx - Ice Light Collision A2.mp3");
	bh->AddCollisionSound("./audio/AngryBird/Sfx - Ice Light Collision A3.mp3");
	bh->AddCollisionSound("./audio/AngryBird/Sfx - Ice Light Collision A4.mp3");
	bh->AddCollisionSound("./audio/AngryBird/Sfx - Ice Light Collision A5.mp3");
	bh->AddCollisionSound("./audio/AngryBird/Sfx - Ice Light Collision A6.mp3");
	bh->AddCollisionSound("./audio/AngryBird/Sfx - Ice Light Collision A7.mp3");
	bh->AddCollisionSound("./audio/AngryBird/Sfx - Ice Light Collision A8.mp3");

	bh->AddDamageSound("./audio/AngryBird/Sfx - Jewel Break 01b.mp3");
	bh->AddDamageSound("./audio/AngryBird/Sfx - Jewel Break 02b.mp3");
	bh->AddDamageSound("./audio/AngryBird/Sfx - Jewel Break 03b.mp3");

	bh->AddBreakSound("./audio/AngryBird/Sfx - Jewel Break 01.mp3");
	bh->AddBreakSound("./audio/AngryBird/Sfx - Jewel Break 02.mp3");
	bh->AddBreakSound("./audio/AngryBird/Sfx - Jewel Break 03.mp3");

	return rectEntity;
}

buki::Entity* buki::Bird::Clone(Vector2 position, Vector2 size, float rotation)
{
	number++;
	std::string name = "bird" + std::to_string(number);
	Entity* bird = Engine::GetInstance().World().Create(name);
	bird->GetTransform()->SetPosition(position);
	bird->GetTransform()->SetSize(size);
	bird->GetTransform()->SetRotation(rotation);
	bird->SetZ(1);
	Sprite* birdImg = bird->AddComponent<Sprite>();
	birdImg->Load("./assets/Kenney/Physics Assets/PNG/Aliens/alienGreen_round.png");
	Circle* col = bird->AddComponent<Circle>();
	col->Collider.Density = 3.0f;
	col->Collider.Friction = 0.3f;
	col->Collider.Restitution = 0.5f;
	col->Collider.Radius = bird->GetTransform()->GetSize().x / 2.0f;

	bird->AddComponent<AngryBirdController>();

	bird->SetLayer("Projectile");

	return bird;
}

buki::Entity* buki::ButtonSimple::Clone(Vector2 position, Vector2 size, float rotation)
{
	number++;
	std::string name = "ButtonSimple" + std::to_string(number);
	Entity* button = Engine::GetInstance().World().Create(name);
	//button->SetZ(10);
	Button* buttonCmp = button->AddComponent<Button>();
	button->GetTransform()->SetSize(size);
	button->GetTransform()->SetPosition(position);
	buttonCmp->SetbuttonFont("./assets/Kenney/Fonts/Kenney Blocks.ttf");
	buttonCmp->SetButtonText("Button Simple", 24);
	buttonCmp->SetImage("./assets/Kenney/MiniGame/Ui/miniguide5_button.png");
	buttonCmp->GetText()->SetColor(Color::BLACK);
	buttonCmp->GetText()->SetPosition({ 0.0f, -0.15f });
	buttonCmp->SetFitToText(false);
	return button;
}

buki::Entity* buki::PauseScreen::Clone(Vector2 position, Vector2 size, float rotation)
{
	number++;
	std::string name = "pauseScreen" + std::to_string(number);
	Entity* screen = Engine::GetInstance().World().Create(name);
	Sprite* sprite = screen->AddComponent<Sprite>();
	Color color = Color::GREY;
	color = color - Color((uchar)0.0f, (uchar)0.0f, (uchar)0.0f, (uchar)0.6f);
	sprite->SetColor(color);
	Vector2 screenSize = Vector2();
	Engine::GetInstance().Graphics().GetWindowSize(&screenSize.x, &screenSize.y);
	sprite->SetSize(screenSize);
	return screen;
}
