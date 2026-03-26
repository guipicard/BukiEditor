#include "Engine.h"
#include "Text.h"
#include "Entity.h"

buki::Text::Text(Entity* _entity)
	: Sprite(_entity)
{
	backgroundColor = Color::BLACK;
	//textPos = Vector2();
}

buki::Text::~Text()
{
}

void buki::Text::Draw(float alpha)
{
	Vector2 pos, size;
	pos = m_Entity->GetTransform()->GetPosition() + positionOffset;
	size = m_Entity->GetTransform()->GetPosition() + sizeOffset;

	if (m_Background)
	{
		float s;
		Graphics().GetScale(&s);
		Vector2 offsetScaled{ backgroundOffset / s };
		Vector2 bgSize = textSize + offsetScaled;
		offsetScaled /= 2;
		Vector2 bgPos{pos.x, pos.y};
		Engine::GetInstance().Graphics().FillRect(RectF{ bgPos.x, bgPos.y, bgSize.x , bgSize.y }, backgroundColor);
	}
	Graphics().DrawString(m_Text, m_Id, pos.x, pos.y, textSize.x, textSize.y, m_Color);
}

void buki::Text::SetTextScalable(std::string _text)
{
	m_Text = _text;
	float w, h;
	Graphics().GetTextSizeFixed(_text, m_Id, &w, &h);
	textSize = Vector2(w, h);
}

void buki::Text::SetTextFixed(std::string _text)
{
	m_Text = _text;
	float w, h;
	Graphics().GetTextSize(_text, m_Id, &w, &h);
	textSize = Vector2(w, h);
}

std::string& buki::Text::GetText() const
{
	std::string& text = const_cast<std::string&>(m_Text);
	return text;
}

void buki::Text::SetText(std::string _text)
{
	if (fixed)
	{
		SetTextFixed(_text);
	}
	else
	{
		SetTextScalable(_text);
	}
}

json buki::Text::Serialize()
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
	doc["backgroundOffset"]["x"] = backgroundOffset.x;
	doc["backgroundOffset"]["y"] = backgroundOffset.y;

	doc["Text"] = m_Text;
	doc["Background"] = m_Background;
	doc["TextSize"] = { textSize.x, textSize.y };
	doc["BackgroundColor"] = { backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a };
	doc["fontSize"] = fontSize;
	doc["fixed"] = fixed;
	return doc;
}

void buki::Text::Deserialize(json _doc)
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
	fixed = _doc["fixed"].get<bool>();
	backgroundOffset.x = _doc["backgroundOffset"]["x"].get<float>();
	backgroundOffset.y = _doc["backgroundOffset"]["y"].get<float>();

	fontSize = _doc["fontSize"].get<int>();
	m_Background = _doc["Background"].get<bool>();
	std::vector<float> sizeData = _doc["TextSize"].get<std::vector<float>>();
	textSize = Vector2(sizeData[0], sizeData[1]);
	std::vector<uchar> colorData = _doc["BackgroundColor"].get<std::vector<uchar>>();
	backgroundColor = Color(colorData[0], colorData[1], colorData[2], colorData[3]);
	m_Text = _doc["Text"].get<std::string>();
}

void buki::Text::Set()
{
	if (path == "") return;
	LoadText(path, fontSize);
	SetTextScalable(m_Text);
}

void buki::Text::SetFixed(const bool state)
{
	if (state != fixed)
	{
		fixed = state;
		SetText(m_Text);
	}
}

void buki::Text::LoadText(const std::string& _path, int size)
{
	path = _path;
	fontSize = size;
	m_Id = Graphics().LoadFont(_path, size);
}
