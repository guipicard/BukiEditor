#include "ShapesDemo.h"

#include "Engine.h"
#include "Animation.h"
#include "Spawner.h"
#include "Prototypes.h"
#include "Box.h"
#include "Circle.h"
#include "Sprite.h"
#include "RigidBody.h"
#include "Text.h"
#include "ShapesController.h"
#include "Button.h"
#include "Camera2D.h"
#include "TileLayer.h"

using namespace buki;

ShapesDemo::ShapesDemo()
{
	Ground = nullptr;
	GroundSprite = nullptr;
	Background = nullptr;
	BackgroundImage = nullptr;
}

void buki::ShapesDemo::CodeLoad()
{
	Engine::Get().Graphics().SetCameraPosition(0.0f, 0.0f);

	Ground = Instantiate("Ground");
	RigidBody* groundRB = Ground->AddComponent<RigidBody>();
	Box* groundCollider = Ground->AddComponent<Box>();

	GroundAndBackground();

	groundRB->def.type = RigidBodyDef::BodyType::Static;
	groundRB->def.motionLocks.linearX = true;
	groundRB->def.motionLocks.linearY = true;
	groundRB->def.motionLocks.angularZ = true;

	groundCollider->def.density = 1.0f;
	groundCollider->def.friction = 0.3f;
	groundCollider->def.size = Ground->T()->GetSize() / 2.0f;
	groundCollider->def.fillDraw = true;
	groundCollider->def.shapeDraw = true;
	groundCollider->def.filter = 0;

	Ground->ActivatePhysics();

	Entity* controllerEntity = Instantiate("controller");
	controllerEntity->AddComponent<ShapesController>();

	Vector2 buttonsSize = Vector2(1.0f, 0.5f);
	Camera2D camera = Engine::Get().GetActiveCamera();
	Vector2 cameraSize;
	camera.GetViewportWorldSize(&cameraSize.x, &cameraSize.y);

	Vector2 buttonPos = { -cameraSize / 2.0f };
	buttonPos += { 1.0f, 1.0f };

	std::string fontpath = "./fonts/droid_sans.ttf";

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

	Entity* boxBothEntity = Instantiate("BoxBoth");
	boxBothEntity->T()->SetSize(buttonsSize);
	boxBothEntity->T()->SetPosition(buttonPos);
	Button* boxBothButton = boxBothEntity->AddComponent<Button>();
	boxBothButton->SetStyle(style);
	boxBothButton->SetFontPath(fontpath);
	boxBothButton->SetText("Box Both");
	boxBothButton->SetFontSize(18);
	boxBothButton->SetMessage("BoxBoth");

	buttonPos.x += 3.0f;

	Entity* boxDrawEntity = Instantiate("BoxDraw");
	boxDrawEntity->T()->SetSize(buttonsSize);
	boxDrawEntity->T()->SetPosition(buttonPos);
	Button* boxDrawButton = boxDrawEntity->AddComponent<Button>();
	boxDrawButton->SetStyle(style);
	boxDrawButton->SetFontPath(fontpath);
	boxDrawButton->SetText("Box Draw");
	boxDrawButton->SetFontSize(18);
	boxDrawButton->SetMessage("BoxDraw");

	buttonPos.x += 3.0f;

	Entity* boxFillEntity = Instantiate("BoxFill");
	boxFillEntity->T()->SetSize(buttonsSize);
	boxFillEntity->T()->SetPosition(buttonPos);
	Button* boxFillButton = boxFillEntity->AddComponent<Button>();
	boxFillButton->SetStyle(style);
	boxFillButton->SetFontPath(fontpath);
	boxFillButton->SetText("Box Fill");
	boxFillButton->SetFontSize(18);
	boxFillButton->SetMessage("BoxFill");

	buttonPos.x -= 6.0f;
	buttonPos.y += 3.0f;

	Entity* circleBothEntity = Instantiate("CircleBoth");
	circleBothEntity->T()->SetSize(buttonsSize);
	circleBothEntity->T()->SetPosition(buttonPos);
	Button* circleBothButton = circleBothEntity->AddComponent<Button>();
	circleBothButton->SetStyle(style);
	circleBothButton->SetFontPath(fontpath);
	circleBothButton->SetText("Circle Both");
	circleBothButton->SetFontSize(18);
	circleBothButton->SetMessage("CircleBoth");

	buttonPos.x += 3.0f;

	Entity* circleDrawEntity = Instantiate("CircleDraw");
	circleDrawEntity->T()->SetSize(buttonsSize);
	circleDrawEntity->T()->SetPosition(buttonPos);
	Button* circleDrawButton = circleDrawEntity->AddComponent<Button>();
	circleDrawButton->SetStyle(style);
	circleDrawButton->SetFontPath(fontpath);
	circleDrawButton->SetText("Circle Draw");
	circleDrawButton->SetFontSize(18);
	circleDrawButton->SetMessage("CircleDraw");

	buttonPos.x += 3.0f;

	Entity* circleFillEntity = Instantiate("CircleFill");
	circleFillEntity->T()->SetSize(buttonsSize);
	circleFillEntity->T()->SetPosition(buttonPos);
	Button* circleFillButton = circleFillEntity->AddComponent<Button>();
	circleFillButton->SetStyle(style);
	circleFillButton->SetFontPath(fontpath);
	circleFillButton->SetText("Circle Fill");
	circleFillButton->SetFontSize(18);
	circleFillButton->SetMessage("CircleFill");

	SaveScene();
}

void buki::ShapesDemo::OnWindowResize()
{
}

void buki::ShapesDemo::OnNotify(const std::string& button)
{
	ShapesController* controller = Engine::Get().World().FindEntityByName("controller")->GetComponent<ShapesController>();

	if (button == "BoxBoth")
	{
		controller->DrawBoxFill(true);
		controller->DrawBoxShape(true);

		for (auto e : Engine::Get().World().GetEntitiesInWorld())
		{
			if (e->GetComponent<TileLayer>()) continue;

			Box* box = e->GetComponent<Box>();
			if (box && box->def.filter != 8 && box->def.filter != 1)
			{
				box->def.fillDraw = true;
				box->def.shapeDraw = true;
			}
		}
	}

	if (button == "BoxDraw")
	{
		controller->DrawBoxFill(false);
		controller->DrawBoxShape(true);

		for (auto e : Engine::Get().World().GetEntitiesInWorld())
		{
			if (e->GetComponent<TileLayer>()) continue;

			Box* box = e->GetComponent<Box>();
			if (box && box->def.filter != 8 && box->def.filter != 1)
			{
				box->def.fillDraw = false;
				box->def.shapeDraw = true;
			}
		}
	}

	if (button == "BoxFill")
	{
		controller->DrawBoxFill(true);
		controller->DrawBoxShape(false);

		for (auto e : Engine::Get().World().GetEntitiesInWorld())
		{
			if (e->GetComponent<TileLayer>()) continue;

			Box* box = e->GetComponent<Box>();
			if (box && box->def.filter != 8 && box->def.filter != 1)
			{
				box->def.fillDraw = true;
				box->def.shapeDraw = false;
			}
		}
	}

	if (button == "CircleBoth")
	{
		controller->DrawCircleFill(true);
		controller->DrawCircleShape(true);

		for (auto e : Engine::Get().World().GetEntitiesInWorld())
		{
			if (e->GetComponent<TileLayer>()) continue;

			Circle* circle = e->GetComponent<Circle>();
			if (circle && circle->def.filter != 8 && circle->def.filter != 1)
			{
				circle->def.fillDraw = true;
				circle->def.shapeDraw = true;
			}
		}
	}

	if (button == "CircleDraw")
	{
		controller->DrawCircleFill(false);
		controller->DrawCircleShape(true);

		for (auto e : Engine::Get().World().GetEntitiesInWorld())
		{
			if (e->GetComponent<TileLayer>()) continue;

			Circle* circle = e->GetComponent<Circle>();
			if (circle && circle->def.filter != 8 && circle->def.filter != 1)
			{
				circle->def.fillDraw = false;
				circle->def.shapeDraw = true;
			}
		}
	}

	if (button == "CircleFill")
	{
		controller->DrawCircleFill(true);
		controller->DrawCircleShape(false);

		for (auto e : Engine::Get().World().GetEntitiesInWorld())
		{
			if (e->GetComponent<TileLayer>()) continue;

			Circle* circle = e->GetComponent<Circle>();
			if (circle && circle->def.filter != 8 && circle->def.filter != 1)
			{
				circle->def.fillDraw = true;
				circle->def.shapeDraw = false;
			}
		}
	}
}

void buki::ShapesDemo::GroundAndBackground()
{
	Engine::Get().Log().LogMessage("shapes");

	float intWindowW, intWindowH;
	Engine::Get().GetActiveCamera().GetViewportWorldSize(&intWindowW, &intWindowH);
	Vector2 windowSize = Vector2(intWindowW, intWindowH);
	const float groundWidth = windowSize.x - 6.0f;
	const Vector2 bgTileSize = { windowSize.y, windowSize.y };
	const Vector2 groundTileSize = { 1.0f, 1.0f };
	const Vector2 groundPos = { 0.0f, windowSize.y / 2.0f - (groundTileSize.y / 2.0f) };
	const int tileNum = static_cast<int>(groundWidth / groundTileSize.x);

	Ground->T()->SetPosition(groundPos);

	if (GroundSprite)
	{

		GroundSprite->ClearTiles();
		GroundSprite->SetDefaultTileSize(groundTileSize);

		RectF groundSourceRect{ 0.0f, 0.0f, static_cast<float>(GroundSprite->GetTexture()->width), static_cast<float>(GroundSprite->GetTexture()->height) };

		GroundSprite->BuildUniformStrip(
			tileNum,
			Vector2{ -groundWidth * 0.5f + groundTileSize.x * 0.5f, 0.0f },
			Vector2{ groundTileSize.x, 0.0f },
			groundTileSize,
			groundSourceRect
		);

	}
	Ground->T()->SetSize(Vector2(groundTileSize.x * tileNum, groundTileSize.y));
	if (BackgroundImage == nullptr )
	{
		Engine::Get().Log().LogError("Failed to load background or ground texture.");
		return;
	}
	Background->T()->SetPosition({ 0.0f,0.0f });


	BackgroundImage->ClearTiles();
	BackgroundImage->SetDefaultTileSize(bgTileSize);

	RectF bgSourceRect{ 0.0f, 0.0f, static_cast<float>(BackgroundImage->GetTexture()->width), static_cast<float>(BackgroundImage->GetTexture()->height) };
	BackgroundImage->BuildUniformStrip(2, Vector2{ -bgTileSize.x * 0.5f, 0.0f }, Vector2{ bgTileSize.x, 0.0f }, bgTileSize, bgSourceRect);

}