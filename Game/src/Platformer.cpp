#pragma once
#include "Platformer.h"
#include "Entity.h"
#include "TileLayer.h"
#include "RigidBody.h"
#include "Box.h"
#include "AnimTest.h"
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
	buki::Engine::Get().Graphics().SetCameraPosition(0.0f, 0.0f);
	Background = Instantiate("Background");
	BackgroundAtlas = Background->AddComponent<TileLayer>();
	BackgroundAtlas->SetAtlasPath("/assets/Kenney/Physics Assets/PNG/Backgrounds/colored_shroom.png");

	Ground = Instantiate("Ground");
	Ground->SetLayer("Ground");
	RigidBody* GroundRB = Ground->AddComponent<RigidBody>();
	Box* GroundCollider = Ground->AddComponent<Box>();
	GroundAtlas = Ground->AddComponent<TileLayer>();
	GroundAtlas->SetAtlasPath("/assets/Kenney/Physics Assets/PNG/Other/dirt.png");

	float intWindowW, intWindowH;
	buki::Engine::Get().GetActiveCamera().GetViewportWorldSize(&intWindowW, &intWindowH);
	Vector2 windowSize = Vector2(intWindowW, intWindowH);
	RectF dest = RectF{windowSize.x / 2.0f, windowSize.y, windowSize.x, 1.0f};
	dest.y -= dest.h / 2;

	GroundAtlas->Set();
	BackgroundAtlas->Set();

	GroundAndBackground();

	PlayerEntity = Instantiate("AnimTest");
	PlayerEntity->SetZ(3);
	AnimTest* playerCmp = PlayerEntity->AddComponent<AnimTest>();
}

void buki::Platformer::OnWindowResize()
{
}

void buki::Platformer::OnNotify(const std::string& button)
{
}

void buki::Platformer::GroundAndBackground()
{
	float intWindowW, intWindowH;
	Engine::Get().GetActiveCamera().GetViewportWorldSize(&intWindowW, &intWindowH);
	Vector2 windowSize = Vector2(intWindowW, intWindowH);

	Vector2 bgTileSize = { windowSize.y, windowSize.y };
	Vector2 BGPos = { 0.0f, 0.0f };
	Vector2 bgSpriteSize = { BackgroundAtlas->GetTexture()->width, BackgroundAtlas->GetTexture()->height };
	
	BackgroundAtlas->SetDefaultTileSize(bgTileSize);
	BackgroundAtlas->ClearTiles();
	BackgroundAtlas->AddTile(Vector2{ -bgTileSize.x * 0.5f, 0.0f }, bgTileSize, RectF{ 0.0f, 0.0f, bgSpriteSize.x, bgSpriteSize.y });
	BackgroundAtlas->AddTile(Vector2{ bgTileSize.x * 0.5f, 0.0f }, bgTileSize, RectF{ 0.0f, 0.0f, bgSpriteSize.x, bgSpriteSize.y });
	Background->T()->SetSize(Vector2(bgTileSize.x * 2.0f, bgTileSize.y));
	
	Vector2 groundTileSize = { 1.0f, 1.0f };
	Vector2 groundPos = { 0.0f, (windowSize.y - (groundTileSize.y*3)) / 2 };
	Background->T()->SetSize(Vector2(bgTileSize.x * 2, bgTileSize.y));
	int tileNum = static_cast<int>(windowSize.x / groundTileSize.x);
	Vector2 groundSize = { groundTileSize.x * tileNum, groundTileSize.y };

	Ground->T()->SetPosition(groundPos);
	Ground->T()->SetSize(Vector2(groundTileSize.x * tileNum, groundTileSize.y));
	GroundAtlas->SetDefaultTileSize(groundTileSize);
	GroundAtlas->ClearTiles();

	GroundAtlas->BuildUniformStrip(
		tileNum,
		Vector2{ (-windowSize.x + groundTileSize.x + (windowSize.x-groundSize.x)) * 0.5f, 0.0f },
		Vector2{ groundTileSize.x, 0.0f },
		groundTileSize,
		RectF{ 0.0f, 0.0f, (float)(GroundAtlas->GetTexture()->width), (float)(GroundAtlas->GetTexture()->height) }
	);

	Ground->T()->SetSize(groundSize);

	RigidBody* GroundRB = Ground->GetComponent<RigidBody>();
	GroundRB->def.type = RigidBody::BodyType::Static;
	GroundRB->def.motionLocks = { true,true,true };
	Box* GroundCollider = Ground->GetComponent<Box>();
	GroundCollider->def.shapeDraw = true;
	GroundCollider->def.restitution = 0.0f;
	GroundCollider->def.friction = 1.0f;
	GroundCollider->def.size = groundSize;

	buki::Engine::Get().Log().LogMessage("Ground size: " + std::to_string(groundSize.x) + " x " + std::to_string(groundSize.y));
	Ground->ActivatePhysics();
}
