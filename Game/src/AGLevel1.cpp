#include "AGLevel1.h"
#include "Engine.h"
#include "Entity.h"
#include "Sprite.h"
#include "RigidBody.h"
#include "Box.h"
#include "Spawner.h"
#include "Prefabs.h"
#include "Button.h"
#include "Text.h"
#include "AngryBirdController.h"
#include "Camera2D.h"
#include "TileLayer.h"

buki::AGLevel1::AGLevel1()
{
	Ground = nullptr;
	GroundSprite = nullptr;
	Background = nullptr;
	BackgroundImage = nullptr;
	spawner = nullptr;
	slignshot1 = nullptr;
	slignshot2 = nullptr;
}

void buki::AGLevel1::CodeLoad()
{
	Engine::Get().Graphics().SetCameraPosition(0.0f, 0.0f);

	Background = Instantiate("Background");
	spawner = Background->AddComponent<Spawner>();
	spawner->AddPrototype("GlassSlim", new GlassSlim());
	BackgroundImage = Background->AddComponent<TileLayer>();
	BackgroundImage->SetAtlasPath("/assets/Kenney/Physics Assets/PNG/Backgrounds/colored_desert.png");

	Ground = Instantiate("Ground");
	RigidBody* groundRB = Ground->AddComponent<RigidBody>();
	Box* groundCollider = Ground->AddComponent<Box>();
	GroundSprite = Ground->AddComponent<TileLayer>();
	GroundSprite->SetAtlasPath("/assets/Kenney/Physics Assets/PNG/Other/grass.png");

	BackgroundImage->Set();
	GroundSprite->Set();

	GroundAndBackground();

	groundRB->def.type = RigidBodyDef::BodyType::Static;
	groundRB->def.motionLocks.linearX = true;
	groundRB->def.motionLocks.linearY = true;
	groundRB->def.motionLocks.angularZ = true;

	groundCollider->def.density = 1.0f;
	groundCollider->def.friction = 0.3f;
	groundCollider->def.size = Ground->T()->GetSize();
	Ground->ActivatePhysics();

	InitLevel();
	SpawnSlignshot(-12.0f);
	CreateLevelMenu();

	//Instantiate("ECEntity")->AddComponent<EditorController>();

	SaveScene();
}

void buki::AGLevel1::OnWindowResize()
{
}

void buki::AGLevel1::OnNotify(const std::string& button)
{
	if (button == "menu")
	{
		Engine::Get().World().SetLoadScene("AngryBird");
	}
	if (button == "reset")
	{
		Engine::Get().World().SetLoadScene("Ag_Lvl_1");
	}
	if (button == "save")
	{
		SaveScene(name + " - DUPLICATE");
	}
}

void buki::AGLevel1::GroundAndBackground()
{
	float intWindowW, intWindowH;
	Engine::Get().GetActiveCamera().GetViewportWorldSize(&intWindowW, &intWindowH);
	Vector2 windowSize = Vector2(intWindowW, intWindowH);

	Vector2 bgTileSize = { windowSize.y, windowSize.y };
	Vector2 groundTileSize = { 1.0f, 1.0f };
	Vector2 BGPos = { 0.0f, 0.0f };
	Vector2 groundPos = { 0.0f, (windowSize.y - groundTileSize.y)/2 };
	Vector2 bgSpriteSize = { BackgroundImage->GetTexture()->width, BackgroundImage->GetTexture()->height };

	Ground->T()->SetPosition(groundPos);
	BackgroundImage->SetDefaultTileSize(bgTileSize);
	BackgroundImage->ClearTiles();
	BackgroundImage->AddTile(Vector2{ -bgTileSize.x * 0.5f, 0.0f }, bgTileSize, RectF{ 0.0f, 0.0f, bgSpriteSize.x, bgSpriteSize.y });
	BackgroundImage->AddTile(Vector2{ bgTileSize.x * 0.5f, 0.0f }, bgTileSize, RectF{ 0.0f, 0.0f, bgSpriteSize.x, bgSpriteSize.y });

	GroundSprite->SetDefaultTileSize(groundTileSize);
	GroundSprite->ClearTiles();

	int tileNum = static_cast<int>(windowSize.x / groundTileSize.x) * 2;
	GroundSprite->BuildUniformStrip(
		tileNum,
		Vector2{ -windowSize.x + groundTileSize.x * 0.5f, 0.0f },
		Vector2{ groundTileSize.x, 0.0f },
		groundTileSize,
		RectF{ 0.0f, 0.0f, (float)(GroundSprite->GetTexture()->width), (float)(GroundSprite->GetTexture()->height) }
	);

	Ground->T()->SetSize(Vector2(groundTileSize.x * tileNum, groundTileSize.y));
	Background->T()->SetSize(Vector2(bgTileSize.x * 2.0f, bgTileSize.y));

}

void buki::AGLevel1::InitLevel()
{
	float intWindowW, intWindowH;
	Engine::Get().GetActiveCamera().GetViewportWorldSize(&intWindowW, &intWindowH);
	Vector2 windowSize = Vector2(intWindowW, intWindowH);
	RectF dest = { windowSize.x / 2.0f, windowSize.y, windowSize.x, 1.0f };
	dest.y -= dest.h / 2.0f;

	const Vector2 size = Vector2(0.75f, 2.25f);
	Vector2 pos = Vector2((windowSize.x / 2.0f) - 6.0f, (windowSize.y / 2.0f) - dest.h - (size.y / 2.0f));
	float rotation = 0.0f;

	spawner->Spawn("GlassSlim", pos, size, rotation);
	pos.x += size.y;
	spawner->Spawn("GlassSlim", pos, size, rotation);
	pos.x += size.y;
	spawner->Spawn("GlassSlim", pos, size, rotation);
	pos.x -= size.y / 2.0f;
	pos.y -= (size.y / 2.0f) + (size.x / 2.0f);
	rotation = 1.57079633f;
	spawner->Spawn("GlassSlim", pos, size, rotation);
	pos.x -= size.y;
	spawner->Spawn("GlassSlim", pos, size, rotation);
	pos.x += size.y;
	pos.y -= (size.y / 2.0f) + (size.x / 2.0f);
	rotation = 0.0f;
	spawner->Spawn("GlassSlim", pos, size, rotation);
	pos.x -= size.y;
	spawner->Spawn("GlassSlim", pos, size, rotation);
	pos.x += size.y / 2.0f;
	pos.y -= (size.y / 2.0f) + (size.x / 2.0f);
	rotation = 1.57079633f;
	spawner->Spawn("GlassSlim", pos, size, rotation);
}

void buki::AGLevel1::SpawnSlignshot(float _offset)
{
	float intWindowW, intWindowH;
	Engine::Get().GetActiveCamera().GetViewportWorldSize(&intWindowW, &intWindowH);
	Vector2 windowSize = Vector2(intWindowW, intWindowH);
	RectF dest = { windowSize.x / 2.0f, windowSize.y, windowSize.x, 1.0f };
	dest.y -= dest.h / 2.0f;

	slignshot1 = Instantiate("slignshot1");
	slignshot2 = Instantiate("slignshot2");

	Vector2 slingshotSize = Vector2(2.0f, 4.0f);
	//Vector2 slingshotPos = {0.0f,0.0f};
	Vector2 slingshotPos = Vector2(_offset, (windowSize.y / 2.0f) - dest.h - (slingshotSize.y / 2.0f));

	slignshot1->T()->SetPosition(slingshotPos);
	slignshot1->T()->SetSize(slingshotSize);
	slignshot2->T()->SetPosition(slingshotPos);
	slignshot2->T()->SetSize(slingshotSize);

	Sprite* slingshot1Img = slignshot1->AddComponent<Sprite>();
	Sprite* slingshot2Img = slignshot2->AddComponent<Sprite>();
	slingshot1Img->SetPath("/assets/slignshot1.png");
	slingshot2Img->SetPath("/assets/slignshot2.png");

	slignshot2->SetZ(2);

	slingshotPos.y -= 1.5f;
	slingshotPos.x += 0.18f;

	Entity* anchor = Instantiate("anchor");
	anchor->T()->SetPosition(slingshotPos);

	spawner->AddPrototype("bird", new Bird());
	spawner->Spawn("bird", slingshotPos, Vector2(1.0f, 1.0f), 0.0f);
}

void buki::AGLevel1::CreateLevelMenu()
{
	std::string buttonFont = "./fonts/Kenney/Kenney Blocks.ttf";

	float intWindowW, intWindowH;
	Engine::Get().GetActiveCamera().GetViewportWorldSize(&intWindowW, &intWindowH);
	Vector2 windowSize = Vector2(intWindowW, intWindowH);

	Vector2 buttonsSize = Vector2(0.5f, 0.5f);

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

	Entity* resetButtonEntity = Instantiate("resetButton");
	resetButtonEntity->T()->SetPosition(Vector2(-buttonsSize.y * 3.0f, (-windowSize.y / 2.0f) + (buttonsSize.y * 2.0f)));
	resetButtonEntity->T()->SetSize(buttonsSize);
	Button* resetButton = resetButtonEntity->AddComponent<Button>();
	resetButton->SetStyle(style);
	resetButton->SetFontPath(buttonFont);
	resetButton->SetFontSize(24);
	resetButton->SetText("Reset");
	resetButton->SetMessage("reset");


	Entity* abMenuButtonEntity = Instantiate("abMenuButton");
	abMenuButtonEntity->T()->SetPosition(Vector2(buttonsSize.y * 3.0f, (-windowSize.y / 2.0f) + (buttonsSize.y * 2.0f)));
	abMenuButtonEntity->T()->SetSize(buttonsSize);
	Button* abMenuButton = abMenuButtonEntity->AddComponent<Button>();
	abMenuButton->SetStyle(style);
	abMenuButton->SetFontPath(buttonFont);
	abMenuButton->SetFontSize(24);
	abMenuButton->SetText("menu");
	abMenuButton->SetMessage("menu");

	Entity* saveButtonEntity = Instantiate("SaveButton");
	saveButtonEntity->T()->SetPosition(Vector2(0.0f, (-windowSize.y / 2.0f) + (buttonsSize.y * 4.0f)));
	saveButtonEntity->T()->SetSize(buttonsSize);
	Button* saveButton = saveButtonEntity->AddComponent<Button>();
	saveButton->SetStyle(style);
	saveButton->SetFontPath(buttonFont);
	saveButton->SetFontSize(24);
	saveButton->SetText("save");
	saveButton->SetMessage("save");
}