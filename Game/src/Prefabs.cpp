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
	Entity* circleEntity = Engine::Get().World().Create(name);
	int radius = rand() % 6 + 2;
	float r = (float)radius;
	r /= 4;
	float rot = static_cast<float>((rand() % 7000) / 1000.0f);
	Vector2 randomSize = Vector2(r, r);
	circleEntity->Initialize(position, 0.0f, randomSize);

	Circle* col = circleEntity->AddComponent<Circle>();
	col->def.density = 1.0f;
	col->def.friction = 0.3f;
	col->def.restitution = 0.6f;
	col->def.radius = r / 2;
	col->def.shapeColor = { 0.0f, 0.0f, 1.0f, 1.0f };
	col->def.fillColor = { 1.0f, 0.0f, 0.0f, 1.0f };
	col->def.shapeDraw = true;
	col->def.fillDraw = true;
	col->def.filter = 0;

	circleEntity->ActivatePhysics();

	return circleEntity;
}

buki::Entity* buki::RectangleShape::Clone(Vector2 position, Vector2 size, float rotation)
{
	number++;
	std::string name = "rect" + std::to_string(number);
	Entity* rectEntity = Engine::Get().World().Create(name);
	int sizeX = rand() % 10 + 2;
	int sizeY = rand() % 10 + 2;
	float rot = static_cast<float>((rand() % 7000) / 1000.0f);
	Vector2 randomSize = Vector2(sizeX, sizeY) / 5;
	rectEntity->Initialize(position, rot, randomSize);
	rectEntity->SetZ(1);

	Box* col = rectEntity->AddComponent<Box>();
	col->def.density = 1.0f;
	col->def.friction = 0.3f;
	col->def.restitution = 0.3f;
	col->def.size = randomSize;
	col->def.shapeColor = { 1.0f, 1.0f, 0.0f, 1.0f };
	col->def.fillColor = { 0.0f, 1.0f, 0.0f, 1.0f };
	col->def.shapeDraw = true;
	col->def.fillDraw = true;
	col->def.filter = 0;

	rectEntity->ActivatePhysics();

	return rectEntity;
}

buki::Entity* buki::StoneSlim::Clone(Vector2 position, Vector2 size, float rotation)
{
	number++;
	std::string name = "StoneSlim" + std::to_string(number);
	Entity* rectEntity = Engine::Get().World().Create(name);
	rectEntity->Initialize(position, rotation, size);

	rectEntity->AddComponent<Sprite>();

	Box* col = rectEntity->AddComponent<Box>();
	col->def.density = 5.0f;
	col->def.friction = 0.8f;
	col->def.restitution = 0.3f;
	col->def.size = size;

	rectEntity->ActivatePhysics();

	BricksBehaviour* bh = rectEntity->AddComponent<BricksBehaviour>();
	bh->SetMaxHealth(100.0f);

	bh->AddImage("/assets/Kenney/Physics Assets/PNG/Stone elements/elementStone020.png");
	bh->AddImage("/assets/Kenney/Physics Assets/PNG/Stone elements/elementStone025.png");
	bh->AddImage("/assets/Kenney/Physics Assets/PNG/Stone elements/elementStone052.png");

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
	Entity* rectEntity = Engine::Get().World().Create(name);
	rectEntity->Initialize(position, rotation, size);

	Sprite* m_Sprite = rectEntity->AddComponent<Sprite>();

	Box* col = rectEntity->AddComponent<Box>();
	col->def.density = 3.0f;
	col->def.friction = 0.4f;
	col->def.restitution = 0.3f;
	col->def.size = size;

	rectEntity->ActivatePhysics();

	BricksBehaviour* bh = rectEntity->AddComponent<BricksBehaviour>();
	bh->SetMaxHealth(75.0f);

	bh->AddImage("/assets/Kenney/Physics Assets/PNG/Wood elements/elementWood019.png");
	bh->AddImage("/assets/Kenney/Physics Assets/PNG/Wood elements/elementWood024.png");
	bh->AddImage("/assets/Kenney/Physics Assets/PNG/Wood elements/elementWood051.png");

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
	Entity* rectEntity = Engine::Get().World().Create(name);
	rectEntity->Initialize(position, rotation, size);

	Sprite* m_Sprite = rectEntity->AddComponent<Sprite>();

	Box* col = rectEntity->AddComponent<Box>();
	col->def.density = 1.0f;
	col->def.friction = 0.1f;
	col->def.restitution = 0.3f;
	col->def.size = size;

	rectEntity->ActivatePhysics();

	BricksBehaviour* bh = rectEntity->AddComponent<BricksBehaviour>();
	bh->SetMaxHealth(50.0f);

	bh->AddImage("/assets/Kenney/Physics Assets/PNG/Glass elements/elementGlass021.png");
	bh->AddImage("/assets/Kenney/Physics Assets/PNG/Glass elements/elementGlass026.png");
	bh->AddImage("/assets/Kenney/Physics Assets/PNG/Glass elements/elementGlass053.png");

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
	Entity* bird = Engine::Get().World().Create(name);
	bird->T()->SetPosition(position);
	bird->T()->SetSize(size);
	bird->T()->SetRotation(rotation);
	bird->SetZ(1);
	Sprite* birdImg = bird->AddComponent<Sprite>();
	birdImg->SetPath("/assets/Kenney/Physics Assets/PNG/Aliens/alienGreen_round.png");
	birdImg->Set();
	Circle* col = bird->AddComponent<Circle>();
	//col->def.density = 3.0f;
	col->def.friction = 0.3f;
	col->def.restitution = 0.5f;
	col->def.radius = bird->T()->GetSize().x / 2.0f;

	bird->AddComponent<AngryBirdController>();

	bird->SetLayer("Projectile");

	return bird;
}

buki::Entity* buki::ButtonSimple::Clone(Vector2 position, Vector2 size, float rotation)
{
	number++;
	std::string name = "ButtonSimple" + std::to_string(number);
	ButtonStyle style{
		{ 0.0f, 0.0f, 0.0f, 1.0f },
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
	Entity* button = Engine::Get().World().Create(name);
	//button->SetZ(10);
	Button* buttonCmp = button->AddComponent<Button>();
	button->T()->SetSize(size);
	button->T()->SetPosition(position);
	buttonCmp->SetStyle(style);
	buttonCmp->SetBackgroundImage("/assets/Kenney/MiniGame/Ui/miniguide5_button.png");
	buttonCmp->SetFontPath("/fonts/Kenney/Kenney Blocks.ttf");
	buttonCmp->SetFontSize(36);
	buttonCmp->SetText("Button Simple");
	return button;
}

buki::Entity* buki::PauseScreen::Clone(Vector2 position, Vector2 size, float rotation)
{
	number++;
	std::string name = "pauseScreen" + std::to_string(number);
	Entity* screen = Engine::Get().World().Create(name);
	Sprite* sprite = screen->AddComponent<Sprite>();
	Color color = { 0.3f, 0.3f, 0.3f, 1.0f };
	sprite->SetColor(color);
	Vector2 screenSize = Vector2();
	Engine::Get().GetActiveCamera().GetViewportWorldSize(&screenSize.x, &screenSize.y);
	//sprite->SetSize(screenSize);
	return screen;
}
