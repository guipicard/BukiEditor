#pragma once
#include "Platformer.h"
#include "Entity.h"
#include "Atlas.h"
#include "RigidBody.h"
#include "Box.h"
#include "Player.h"
#include "Animation.h"
#include "Circle.h"
#include "Engine.h"

buki::Platformer::Platformer()
{
	PlayerEntity = nullptr;
	Ground = nullptr;
	GroundAtlas = nullptr;
	Background = nullptr;
	BackgroundAtlas = nullptr;
}

void buki::Platformer::CodeLoad()
{
	buki::Engine::GetInstance().Graphics().SetCameraPosition(Vector2::ZERO);
	Background = Instantiate("Background");
	BackgroundAtlas = Background->AddComponent<Atlas>();
	BackgroundAtlas->Load("assets/Kenney/Physics Assets/PNG/Backgrounds/colored_shroom.png");
	Ground = Instantiate("Ground");
	Ground->SetLayer("Ground");
	RigidBody* GroundRB = Ground->AddComponent<RigidBody>();
	Box* GroundCollider = Ground->AddComponent<Box>();
	GroundAtlas = Ground->AddComponent<Atlas>();
	GroundAtlas->Load("assets/Kenney/Physics Assets/PNG/Other/dirt.png");

	float intWindowW, intWindowH;
	buki::Engine::GetInstance().Graphics().GetWindowSize(&intWindowW, &intWindowH);
	Vector2 windowSize = Vector2(intWindowW, intWindowH);
	RectF dest = RectF(windowSize.x / 2.0f, windowSize.y, windowSize.x, 1.0f);
	dest.y -= dest.h / 2;

	GroundAndBackground();

	GroundRB->Type = RigidBody::BodyType::Static;
	GroundRB->motionLocks = { true,true,true };
	GroundCollider->Collider.material.restitution = 0.0f;
	GroundCollider->Collider.material.friction = 1.0f;
	GroundCollider->Collider.Size = Ground->GetTransform()->GetSize() / 2;

	Ground->ActivatePhysics();

	PlayerEntity = Instantiate("Player");
	//Vector2 playerSpawnPos = Vector2(dest.x, dest.y - dest.h - (1.9f));
	PlayerEntity->GetTransform()->SetPosition(Vector2::ZERO);
	PlayerEntity->GetTransform()->SetSize({ 5.0f, 5.0f });
	Player* playerCmp = PlayerEntity->AddComponent<Player>();

	Ground->ActivatePhysics();
}

void buki::Platformer::OnWindowResize()
{
}

void buki::Platformer::GroundAndBackground()
{
	float intWindowW, intWindowH;
	buki::Engine::GetInstance().Graphics().GetWindowSize(&intWindowW, &intWindowH);
	Vector2 windowSize = Vector2(intWindowW, intWindowH);
	Background->GetTransform()->SetPosition(Vector2(0, 0));
	Vector2 bgTileSize = { windowSize.y, windowSize.y };
	Vector2 groundTileSize = { 1.0f, 1.0f };
	Vector2 groundPos = { 0.0f, (windowSize.y / 2) - (groundTileSize.y / 2) };
	//Vector2 groundPos = { windowSize.x / 2.0f, windowSize.y - (groundTileSize.y / 2) };

	BackgroundAtlas->SetSize(bgTileSize);
	int imgX, imgY;
	BackgroundAtlas->GetTextureSize(&imgX, &imgY);
	BackgroundAtlas->AddSource("bg", RectI{ 0,0, imgX, imgY });
	BackgroundAtlas->SetTileSize(2, 1);
	BackgroundAtlas->AddDestination("bg", 0, 0);
	BackgroundAtlas->AddDestination("bg", 1, 0);

	Ground->GetTransform()->SetPosition(groundPos);
	GroundAtlas->SetSize(groundTileSize);
	GroundAtlas->GetTextureSize(&imgX, &imgY);
	GroundAtlas->AddSource("ground", RectI{ 0,0, imgX, imgY });
	int tileNum = (int)(windowSize.x / groundTileSize.x) *3;
	GroundAtlas->SetTileSize(tileNum, 1);
	Ground->GetTransform()->SetSize(Vector2(groundTileSize.x * tileNum, groundTileSize.y));
	Background->GetTransform()->SetSize(Vector2(bgTileSize.x * 2, bgTileSize.y));
	for (int i = 0; i < tileNum; i++)
	{
		GroundAtlas->AddDestination("ground", i, 0);
	}
}
