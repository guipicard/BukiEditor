#include "AGLevel3.h"
#include "Engine.h"
#include "Entity.h"
#include "Sprite.h"
#include "Atlas.h"
#include "RigidBody.h"
#include "Box.h"
#include "CircleCollider.h"
#include "Spawner.h"
#include "Prefabs.h"
#include "Button.h"
#include "Text.h"
#include "AngryBirdController.h"

buki::AGLevel3::AGLevel3()
{
	Ground = nullptr;
	GroundSprite = nullptr;
	Background = nullptr;
	BackgroundImage = nullptr;
	spawner = nullptr;
	slignshot1 = nullptr;
	slignshot2 = nullptr;
}

void buki::AGLevel3::CodeLoad()
{
	buki::Engine::GetInstance().Graphics().SetCameraPosition(Vector2::ZERO);
	Background = Instantiate("Background");
	spawner = Background->AddComponent<Spawner>();
	spawner->AddPrototype("StoneSlim", new StoneSlim());
	BackgroundImage = Background->AddComponent<Atlas>();
	BackgroundImage->Load("assets/Kenney/Physics Assets/PNG/Backgrounds/colored_desert.png");

	Ground = Instantiate("Ground");
	RigidBody* GroundRB = Ground->AddComponent<RigidBody>();
	Box* GroundCollider = Ground->AddComponent<Box>();
	GroundSprite = Ground->AddComponent<Atlas>();
	GroundSprite->Load("assets/Kenney/Physics Assets/PNG/Other/grass.png");

	GroundAndBackground();

	GroundRB->Type = RigidBody::BodyType::Static;
	GroundRB->FixedRotation = true;
	GroundCollider->Collider.Density = 1.0f;
	GroundCollider->Collider.Friction = 0.3f;
	GroundCollider->Collider.Size = Ground->GetTransform()->GetSize() / 2;
	Ground->ActivatePhysics();

	InitLevel();
	SpawnSlignshot(-12.0f);
	CreateLevelMenu();

	SaveScene();
}

void buki::AGLevel3::OnWindowResize()
{
}

void buki::AGLevel3::OnNotify(const std::string& button)
{
	if (button == "menu")
	{
		buki::Engine::GetInstance().World().SetLoadScene("AngryBird");
	}
	if (button == "reset")
	{
		buki::Engine::GetInstance().World().SetLoadScene("Ag_Lvl_3");
	}
}

void buki::AGLevel3::GroundAndBackground()
{
	float intWindowW, intWindowH;
	buki::Engine::GetInstance().Graphics().GetWindowSize(&intWindowW, &intWindowH);
	Vector2 windowSize = Vector2(intWindowW, intWindowH);
	//Background->GetTransform()->SetPosition(Vector2(windowSize.x / 2, windowSize.y / 2));
	Vector2 bgTileSize = { windowSize.y, windowSize.y };
	Vector2 groundTileSize = { 1.0f, 1.0f };
	Vector2 groundPos = { 0.0f, (windowSize.y / 2) - (groundTileSize.y / 2) };

	BackgroundImage->SetSize(bgTileSize);
	int imgX, imgY;
	BackgroundImage->GetTextureSize(&imgX, &imgY);
	BackgroundImage->AddSource("bg", RectI{ 0,0, imgX, imgY });
	BackgroundImage->SetTileSize(2, 1);
	BackgroundImage->AddDestination("bg", 0, 0);
	BackgroundImage->AddDestination("bg", 1, 0);

	Ground->GetTransform()->SetPosition(groundPos);
	GroundSprite->SetSize(groundTileSize);
	GroundSprite->GetTextureSize(&imgX, &imgY);
	GroundSprite->AddSource("ground", RectI{ 0,0, imgX, imgY });
	int tileNum = (int)(windowSize.x / groundTileSize.x) * 2;
	GroundSprite->SetTileSize(tileNum, 1);
	Ground->GetTransform()->SetSize(Vector2(groundTileSize.x * tileNum, groundTileSize.y));
	Background->GetTransform()->SetSize(Vector2(bgTileSize.x * 2, bgTileSize.y));
	for (int i = 0; i < tileNum; i++)
	{
		GroundSprite->AddDestination("ground", i, 0);
	}
}

void buki::AGLevel3::InitLevel()
{
	float intWindowW, intWindowH;
	buki::Engine::GetInstance().Graphics().GetWindowSize(&intWindowW, &intWindowH);
	Vector2 windowSize = Vector2(intWindowW, intWindowH);
	RectF dest = RectF(windowSize.x / 2.0f, windowSize.y, windowSize.x, 1.0f);
	dest.y -= dest.h / 2;

	const Vector2 size = Vector2(0.75f, 2.25f);
	Vector2 pos = Vector2((windowSize.x / 2) - 6.0f, (windowSize.y / 2) - (dest.h) - (size.y / 2.0f));
	float rotation = 0.0f;
	spawner->Spawn("StoneSlim", pos, size, rotation);
	pos.x += size.y;
	spawner->Spawn("StoneSlim", pos, size, rotation);
	pos.x += size.y;
	spawner->Spawn("StoneSlim", pos, size, rotation);
	pos.x -= (size.y) / 2.0f;
	pos.y -= ((size.y / 2.0f) + (size.x / 2.0f));
	rotation = 1.57079633f;
	spawner->Spawn("StoneSlim", pos, size, rotation);
	pos.x -= size.y;
	spawner->Spawn("StoneSlim", pos, size, rotation);
	pos.x += size.y;
	pos.y -= ((size.y / 2.0f) + (size.x / 2.0f));
	rotation = 0.0f;
	spawner->Spawn("StoneSlim", pos, size, rotation);
	pos.x -= size.y;
	spawner->Spawn("StoneSlim", pos, size, rotation);
	pos.x += size.y / 2.0f;
	pos.y -= (size.y / 2.0f) + (size.x / 2.0f);
	rotation = 1.57079633f;
	spawner->Spawn("StoneSlim", pos, size, rotation);
}

void buki::AGLevel3::SpawnSlignshot(float _offset)
{
	float intWindowW, intWindowH;
	buki::Engine::GetInstance().Graphics().GetWindowSize(&intWindowW, &intWindowH);
	Vector2 windowSize = Vector2(intWindowW, intWindowH);
	RectF dest = RectF(windowSize.x / 2.0f, windowSize.y, windowSize.x, 1.0f);
	dest.y -= dest.h / 2;

	slignshot1 = Instantiate("slignshot1");
	slignshot2 = Instantiate("slignshot2");
	Vector2 slighshotSize = Vector2(2.0f, 4.0f);
	Vector2 slighshotPos = Vector2(_offset, (windowSize.y / 2) - dest.h - (slighshotSize.y / 2));
	slignshot1->GetTransform()->SetPosition(slighshotPos);
	slignshot1->GetTransform()->SetSize(slighshotSize);
	slignshot2->GetTransform()->SetPosition(slighshotPos);
	slignshot2->GetTransform()->SetSize(slighshotSize);
	Sprite* slignshot1Img = slignshot1->AddComponent<Sprite>();
	Sprite* slignshot2Img = slignshot2->AddComponent<Sprite>();
	slignshot1Img->Load("assets/slignshot1.png");
	slignshot2Img->Load("assets/slignshot2.png");
	slignshot2->SetZ(2);

	slighshotPos.y -= 1.5f;
	slighshotPos.x += 0.18f;

	Entity* anchor = Instantiate("anchor");
	anchor->GetTransform()->SetPosition(slighshotPos);

	spawner->AddPrototype("bird", new Bird());
	spawner->Spawn("bird", slighshotPos, Vector2(1.0f, 1.0f), 0.0f);
}

void buki::AGLevel3::CreateLevelMenu()
{
	float intWindowW, intWindowH;
	buki::Engine::GetInstance().Graphics().GetWindowSize(&intWindowW, &intWindowH);
	Vector2 windowSize = Vector2(intWindowW, intWindowH);

	Vector2 buttonsSize = Vector2(0.5f, 0.5f);

	buki::Entity* resetButtonEntity = Instantiate("resetButton");
	Button* resetButton = resetButtonEntity->AddComponent<Button>();
	resetButton->SetMessage("reset");
	resetButton->SetbuttonFont("./fonts/Kenney/Kenney Blocks.ttf");
	resetButton->SetButtonText("reset", 24);
	resetButton->ShowBackground(true);
	resetButton->GetText()->SetColor(Color::BLACK);
	resetButton->SetOutlineColor(Color::BLACK);
	resetButtonEntity->GetTransform()->SetPosition(Vector2(-buttonsSize.y * 3, (-windowSize.y / 2) + (buttonsSize.y * 2)));
	resetButtonEntity->GetTransform()->SetSize(buttonsSize);
	resetButton->OnClick.AddListener(this);
	resetButton->Set();

	buki::Entity* abMenuButtonEntity = Instantiate("abMenuButton");
	Button* abMenuButton = abMenuButtonEntity->AddComponent<Button>();
	abMenuButton->SetMessage("menu");
	abMenuButton->SetbuttonFont("./fonts/Kenney/Kenney Blocks.ttf");
	abMenuButton->SetButtonText("menu", 24);
	abMenuButton->ShowBackground(true);
	abMenuButton->GetText()->SetColor(Color::BLACK);
	abMenuButton->SetOutlineColor(Color::BLACK);
	abMenuButtonEntity->GetTransform()->SetPosition(Vector2(buttonsSize.y * 3, (-windowSize.y / 2) + (buttonsSize.y * 2)));
	abMenuButtonEntity->GetTransform()->SetSize(buttonsSize);
	abMenuButton->OnClick.AddListener(this);
	abMenuButton->Set();
}
