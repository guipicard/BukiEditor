#pragma once
#include "Button.h"
#include "IInput.h"
#include "ICollision.h"
#include "Vector2.h"
#include "Entity.h"
#include "IWorld.h"
#include "IGraphics.h"
#include "ILogger.h"
#include "Text.h"
#include "RigidBody.h"
#include "Box.h"
#include <string>

buki::Button::Button(Entity* entity)
	: Component(entity)
{
	buttonSprite = m_Entity->GetComponent<Sprite>();
	if (buttonSprite == nullptr)
	{
		buttonSprite = m_Entity->AddComponent<Sprite>();
	}
	textContainer = m_Entity->GetComponent<Text>();
	if (textContainer == nullptr)
	{
		textContainer = m_Entity->AddComponent<Text>();
	}
	box = nullptr;
	rb = nullptr;
	textSize = 12;
	textFont = "./fonts/Kenney/Kenney Blocks.ttf";
	buttonText = "Button Text";
	m_Draw = false;
	clicked = false;
	outlineColor = Color::WHITE;
	m_Entity->SetZ(10);
}

void buki::Button::Start()
{
}

void buki::Button::Update(float dt)
{
	Vector2 mousePos;
	Input().GetMousePosition(&mousePos.x, &mousePos.y);
	ShapeId SId = m_Entity->GetComponent<Box>()->GetShapeId();
	if (Physics().TestPoint(SId, mousePos))
	{
		m_Draw = true;
		AABB aabb = Physics().GetPhysicsSize(SId);
		Vector2 phySize = aabb.max - aabb.min;
		if (Input().IsButtonDown(0))
		{
			OnClick.Invoke(message);
		}
	}
	else
	{
		m_Draw = false;
	}
}

void buki::Button::Draw(float alpha)
{
	if (buttonSprite->GetPath() == "")
	{
		box->shapeDraw = m_Draw;
	}
}

void buki::Button::Destroy()
{

}

json buki::Button::Serialize()
{
	json doc;
	doc["Sprite"] = buttonSprite->Serialize();
	doc["Text"] = textContainer->Serialize();
	doc["draw"] = m_Draw;
	doc["buttonText"] = buttonText;
	doc["textFont"] = textFont;
	doc["message"] = message;
	doc["textSize"] = textSize;
	doc["background"] = background;
	doc["outlineColor"]["r"] = outlineColor.r;
	doc["outlineColor"]["g"] = outlineColor.g;
	doc["outlineColor"]["b"] = outlineColor.b;
	doc["outlineColor"]["a"] = outlineColor.a;
	doc["sizeFitToText"] = sizeFitToText;
	return doc;
}

void buki::Button::Deserialize(json _doc)
{
	if (_doc["Sprite"]["path"].get<std::string>() != "")
	{
		buttonSprite->Deserialize(_doc["Sprite"]);
	}
	textContainer->Deserialize(_doc["Text"]);
	m_Draw = _doc["draw"].get<bool>();
	buttonText = _doc["buttonText"].get<std::string>();
	textFont = _doc["textFont"].get<std::string>();
	message = _doc["message"].get<std::string>();
	textSize = _doc["textSize"].get<int>();
	background = _doc["background"].get<bool>();
	outlineColor.r = _doc["outlineColor"]["r"].get<uint8_t>();
	outlineColor.g = _doc["outlineColor"]["g"].get<uint8_t>();
	outlineColor.b = _doc["outlineColor"]["b"].get<uint8_t>();
	outlineColor.a = _doc["outlineColor"]["a"].get<uint8_t>();
	sizeFitToText = _doc["sizeFitToText"].get<bool>();
}

void buki::Button::Set()
{
	if (buttonSprite->GetPath() != "")
	{
		SetImage(buttonSprite->GetPath());
	}
	textContainer->LoadText(textFont, textSize);
	SetButtonText(buttonText, textSize);
	box = m_Entity->GetComponent<Box>();
	if (box == nullptr)
	{
		box = m_Entity->AddComponent<Box>();
	}
	rb = m_Entity->GetComponent<RigidBody>();
	if (rb == nullptr)
	{
		rb = m_Entity->AddComponent<RigidBody>();
	}
	rb->Type = RigidBody::BodyType::Static;
	box->DebugColor = outlineColor;
	box->Sensor = true;
	box->Collider.m_CanDraw = false;
	box->Collider.Size = (GetSize() + textContainer->GetSize()) / 2;
	box->filter = 8;
	m_Entity->ActivatePhysics();
}

buki::Vector2 buki::Button::GetPosition() const
{
	return m_Entity->GetTransform()->GetPosition();
}

buki::Vector2 buki::Button::GetSize() const
{
	return m_Entity->GetTransform()->GetSize();
}

void buki::Button::SetImage(const std::string _path)
{
	buttonSprite->Load(_path);
	buttonSprite->SetSize(GetSize());
}

void buki::Button::SetButtonText(const std::string text, const int s)
{
	textSize = s;
	buttonText = text;
	textContainer->LoadText(textFont, textSize);
	textContainer->SetText(buttonText);
}
