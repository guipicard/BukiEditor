#pragma once
#include "Sprite.h"
#include "Entity.h"
#include "Button.h"
#include "Text.h"

buki::Sprite::Sprite(Entity* _entity)
	: Component(_entity)
{
	sizeOffset = m_Entity->GetTransform()->GetSize();
}

void buki::Sprite::Draw(float alpha)
{
	Transform* state = m_Entity->GetTransform();
	RectF _dst{
		(state->GetPosition().x + positionOffset.x - (sizeOffset.x * 0.5f)),
		(state->GetPosition().y + positionOffset.y - (sizeOffset.y * 0.5f)),
		sizeOffset.x,
		sizeOffset.y
	};
	//Graphics().DrawTexture(m_Id, m_Src, _dst, state->GetRotation().GetRadians(), m_Flip, m_Color);
}

void buki::Sprite::Load(const std::string& _path)
{
	path = _path;
	if (path != "")
	{
		//m_Id = Graphics().LoadTexture(path);
		//Graphics().GetTextureSize(m_Id, &m_Src.w, &m_Src.h);
	}
}

json buki::Sprite::Serialize()
{
	json doc;
	doc["path"] = path;
	doc["position"]["x"] = positionOffset.x;
	doc["position"]["y"] = positionOffset.y;
	doc["size"]["x"] = sizeOffset.x;
	doc["size"]["y"] = sizeOffset.y;
	doc["color"]["r"] = m_Color.r;
	doc["color"]["g"] = m_Color.g;
	doc["color"]["b"] = m_Color.b;
	doc["color"]["a"] = m_Color.a;
	doc["flip"]["h"] = m_Flip.h;
	doc["flip"]["v"] = m_Flip.v;
	return doc;
}

void buki::Sprite::Deserialize(json _doc)
{
	path = _doc["path"].get<std::string>();
	positionOffset.x = _doc["position"]["x"].get<float>();
	positionOffset.y = _doc["position"]["y"].get<float>();
	sizeOffset.x = _doc["size"]["x"].get<float>();
	sizeOffset.y = _doc["size"]["y"].get<float>();
	m_Color.r = _doc["color"]["r"].get<unsigned char>();
	m_Color.g = _doc["color"]["g"].get<unsigned char>();
	m_Color.b = _doc["color"]["b"].get<unsigned char>();
	m_Color.a = _doc["color"]["a"].get<unsigned char>();
	m_Flip.h = _doc["flip"]["h"].get<bool>();
	m_Flip.v = _doc["flip"]["v"].get<bool>();
}

void buki::Sprite::Set()
{
	Load(path);
}

void buki::Sprite::SetColor(const Color& color)
{
	//m_Color.Set(color);
}

void buki::Sprite::SetFlip(bool h, bool v)
{
	m_Flip.h = h;
	m_Flip.v = v;
}
