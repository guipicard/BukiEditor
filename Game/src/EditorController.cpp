//#pragma once
//#include "EditorController.h"
//#include "Engine.h"
//#include "Camera.h"
//#include "Text.h"
//#include "Entity.h"
//#include "ComponentRegistration.h"
//#include "EditorEntity.h"
//
//REGISTER_COMPONENT(EditorController, "EditorController");
//
//buki::EditorController::EditorController(Entity* entity) : MonoBehaviour(entity)
//{
//	Awake();
//}
//
//void buki::EditorController::Awake()
//{
//	bgOffset = { 20.0f, 10.0f };
//	fpsText = m_Entity->AddComponent<Text>();
//	fpsText->LoadText("./fonts/Kenney/Kenney Future Square.ttf", 30);
//	fpsText->SetColor(Color::WHITE);
//	fpsText->SetFixed(true);
//	fpsText->SetText("FPS");
//	fpsText->SetBackground(true);
//	fpsText->SetBackgroundColor(Color(0, 0, 0, 100));
//	fpsText->SetBackgroundOffset(bgOffset);
//	SetTextPosition();
//	selectedEntity = nullptr;
//	m_Entity->SetZ(50);
//}
//
//void buki::EditorController::Start()
//{
//
//}
//
//void buki::EditorController::FixedUpdate(const float dt)
//{
//	
//}
//
//void buki::EditorController::Destroy()
//{
//}
//
//void buki::EditorController::Update(const float dt)
//{
//	int scroll;
//	
//	if (Input().GetMouseScrollUp(&scroll))
//	{
//		float scale = (float)scroll;
//		Graphics().AddScale(scale);
//	}
//	else if (Input().GetMouseScrollDown(&scroll))
//	{
//		float scale = (float)scroll;
//		Graphics().SubScale(scale);
//	}
//	if (Input().IsButtonPressed(1))
//	{
//		movingScreen = true;
//	}
//	if (Input().IsButtonDown(1))
//	{
//		Input().GetMousePosition(&firstMousePos.x, &firstMousePos.y);
//	}
//	if (Input().IsButtonUp(1))
//	{
//		movingScreen = false;
//	}
//	if (movingScreen)
//	{
//		Vector2 cameraPos = Graphics().GetCamera()->GetPosition();
//		Vector2 mousePos;
//		Input().GetMousePosition(&mousePos.x, &mousePos.y);
//		Vector2 mouseDelta = mousePos - firstMousePos;
//		Graphics().GetCamera()->SetPosition(cameraPos - mouseDelta);
//		Input().GetMousePosition(&firstMousePos.x, &firstMousePos.y);
//	}
//	int fps = Engine::GetInstance().GetFps();
//	std::string fpsString = std::to_string(fps);
//	fpsText->SetText(fpsString);
//	SetTextPosition();
//	if (Input().IsKeyDown(EKey::EKEY_H))
//	{
//		float scale;
//		Graphics().SetScale(METRES_TO_PIXELS);
//		Graphics().GetScale(&scale);
//		Graphics().GetCamera()->SetPosition(Vector2(0.0f, 0.0f));
//	}
//	float timeScale = Engine::GetInstance().GetTimeScale();
//
//	if (!canStopTime)
//	{
//		return;
//	}
//	if (Input().IsKeyDown(EKey::EKEY_SPACE))
//	{
//		Engine::GetInstance().SetTimeScale(timeScale == 1.0f ? 0.0f : 1.0f);
//	}
//
//
//	
//
//	if (Input().IsButtonDown(0))
//	{
//		Vector2 mousePos;
//		Input().GetMousePosition(&mousePos.x, &mousePos.y);
//		//ShapeId SId = m_Entity->GetComponent<Box>()->GetShapeId();
//		std::vector<Entity*> hitEntity = std::vector<Entity*>();
//		if (selectedEntity != nullptr) selectedEntity->SetSelected(false);
//		if (Physics().QueryPoint(mousePos, hitEntity, 9))
//		{
//			selectedEntity = hitEntity[0]->GetComponent<EditorEntity>();
//			if (selectedEntity != nullptr) selectedEntity->SetSelected(true);
//		}
//		else
//		{
//			selectedEntity = nullptr;
//		}
//	}
//}
//
//void buki::EditorController::OnCollisionEnter(Entity* other)
//{
//}
//
//void buki::EditorController::OnCollisionExit(Entity* other)
//{
//}
//
//void buki::EditorController::OnCollisionHit(Entity* other)
//{
//}
//
//json buki::EditorController::Serialize()
//{
//	return json();
//}
//
//void buki::EditorController::Deserialize(json _doc)
//{
//}
//
//void buki::EditorController::Set()
//{
//}
//
//void buki::EditorController::SetTextPosition()
//{
//	float camScale;
//	Graphics().GetScale(&camScale);
//	Vector2 camPos = Graphics().GetCamera()->GetPosition();
//	Vector2 camSize = Vector2(Graphics().GetCamera()->width, -Graphics().GetCamera()->height);
//	camSize /= camScale;
//	Vector2 textSize = fpsText->GetSize();
//	textSize /= 2;
//	textSize.y = -textSize.y;
//	Vector2 offset = bgOffset;
//	offset /= 2;
//	offset /= camScale;
//	Vector2 margin{ offset.x, -offset.x };
//	offset.x = textSize.x + offset.x;
//	offset.y = textSize.y - offset.y;
//	Vector2 pos = camPos + (camSize / 2.0f) - offset - margin;
//	fpsText->GetEntity()->GetTransform()->SetPosition(pos);
//}
