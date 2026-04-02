#pragma once
#include "EditorEntity.h"
#include "Engine.h"
#include "Entity.h"
#include "Box.h"
#include "RigidBody.h"
#include <Flip.h>
#include <RectI.h>
#include <vector>

buki::EditorEntity::EditorEntity(Entity* e)
	: Component(e)
{
}

void buki::EditorEntity::Update(float dt)
{
	//if (Input().IsButtonDown(0))
	//{
	//	Vector2 mousePos;
	//	Input().GetMousePosition(&mousePos.x, &mousePos.y);
	//	ShapeId SId = m_Entity->GetComponent<Box>()->GetShapeId();
	//	if (Physics().TestPoint(SId, mousePos))
	//	{
	//		selected = true;
	//	}
	//	else
	//	{
	//		selected = false;
	//	}
	//}
}

void buki::EditorEntity::Draw(float alpha)
{
	for (auto& cmp : componentsData)
	{
		if (cmp.first == "Sprite")
		{
			Transform* state = m_Entity->GetTransform();
			Vector2 m_Position = Vector2(cmp.second["position"]["x"].get<float>(), cmp.second["position"]["y"].get<float>());
			Vector2 m_Size = Vector2(cmp.second["size"]["x"].get<float>(), cmp.second["size"]["y"].get<float>());
			std::string spritePath = cmp.second["path"].get<std::string>();
			size_t m_Id;
			RectI m_Src;
			if (spritePath != "")
			{
				m_Id = Graphics().LoadTexture(spritePath);
				Graphics().GetTextureSize(m_Id, &m_Src.w, &m_Src.h);

				Flip m_Flip = Flip();
				m_Flip.h = cmp.second["flip"]["h"].get<bool>();
				m_Flip.v = cmp.second["flip"]["v"].get<bool>();
				Color m_Color;
				m_Color.r = cmp.second["color"]["r"].get<unsigned char>();
				m_Color.g = cmp.second["color"]["g"].get<unsigned char>();
				m_Color.b = cmp.second["color"]["b"].get<unsigned char>();
				RectF _dst(
					(state->GetPosition().x + m_Position.x - (m_Size.x * 0.5f)),
					(state->GetPosition().y + m_Position.y - (m_Size.y * 0.5f)),
					m_Size.x,
					m_Size.y
				);
				Graphics().DrawTexture(m_Id, m_Src, _dst, state->GetRotation().GetRadians(), m_Flip, m_Color);
			}
		}

		else if (cmp.first == "Text")
		{
			Vector2 pos, size;
			Vector2 m_Position = Vector2(cmp.second["position"]["x"].get<float>(), cmp.second["position"]["y"].get<float>());
			Vector2 m_Size = Vector2(cmp.second["size"]["x"].get<float>(), cmp.second["size"]["y"].get<float>());
			pos = m_Entity->GetTransform()->GetPosition() + m_Position;
			std::string m_Text = cmp.second["Text"].get<std::string>();
			Color m_Color;
			m_Color.r = cmp.second["color"]["r"].get<unsigned char>();
			m_Color.g = cmp.second["color"]["g"].get<unsigned char>();
			m_Color.b = cmp.second["color"]["b"].get<unsigned char>();
			Color backgroundColor;
			std::vector<uchar> colorData = cmp.second["BackgroundColor"].get<std::vector<uchar>>();
			backgroundColor = Color(colorData[0], colorData[1], colorData[2], colorData[3]);
			bool fixed = cmp.second["fixed"].get<bool>();

			size_t m_Id = Graphics().LoadFont(cmp.second["path"].get<std::string>(), cmp.second["fontSize"].get<int>());
			Vector2 textSize;
			Graphics().GetTextSizeFixed(m_Text, m_Id, &textSize.x, &textSize.y);
			bool m_Background = cmp.second["Background"].get<bool>();
			size = m_Entity->GetTransform()->GetSize() + textSize;
			Vector2 backgroundOffset;
			backgroundOffset.x = cmp.second["backgroundOffset"]["x"].get<float>();
			backgroundOffset.y = cmp.second["backgroundOffset"]["y"].get<float>();
			if (m_Background)
			{
				float s;
				Graphics().GetScale(&s);
				Vector2 bgSize = textSize + (backgroundOffset / s);
				Engine::GetInstance().Graphics().FillRect(RectF{ pos.x, pos.y, bgSize.x , bgSize.y }, backgroundColor);
			}
			Graphics().DrawString(m_Text, m_Id, pos.x, pos.y, textSize.x, textSize.y, m_Color);
		}

		else if (cmp.first == "Atlas")
		{
			if (m_Entity->GetName() == "Background")
			{
				int g = 0;
			}
			int tileWidth = cmp.second["tileWidth"].get<int>();
			int tileHeight = cmp.second["tileHeight"].get<int>();
			if (tileWidth + tileHeight < 2) return;
			std::vector<std::vector<RectI>> destinations = std::vector<std::vector<RectI>>();
			destinations = std::vector<std::vector<RectI>>();
			for (int j = 0; j < tileHeight; j++)
			{
				destinations.push_back(std::vector<RectI>());
				for (int i = 0; i < tileWidth; i++)
				{
					destinations[j].push_back(RectI());
				}
			}
			for (int i = 0; i < tileHeight; i++)
			{
				for (int j = 0; j < tileWidth; j++)
				{
					RectI dst = {
						cmp.second["destinations"][std::to_string(i)][std::to_string(j)]["x"].get<int>(),
						cmp.second["destinations"][std::to_string(i)][std::to_string(j)]["y"].get<int>(),
						cmp.second["destinations"][std::to_string(i)][std::to_string(j)]["w"].get<int>(),
						cmp.second["destinations"][std::to_string(i)][std::to_string(j)]["h"].get<int>()
					};
					destinations[i][j] = dst;
				}
			}
			Vector2 entityPosition = m_Entity->GetTransform()->GetPosition();
			Vector2 m_Position = Vector2(cmp.second["position"]["x"].get<float>(), cmp.second["position"]["y"].get<float>());
			Vector2 m_Size = Vector2(cmp.second["size"]["x"].get<float>(), cmp.second["size"]["y"].get<float>());
			size_t m_Id = Graphics().LoadTexture(cmp.second["path"].get<std::string>());
			Vector2 fullSize = { m_Size.x * tileWidth, m_Size.y * tileHeight };
			entityPosition.x += m_Position.x - (fullSize.x * 0.5f);
			entityPosition.y += m_Position.y - (fullSize.y * 0.5f);
			Graphics().DrawTiledTexture(m_Id, destinations, m_Size, entityPosition, Color::WHITE);
		}

		m_Entity->GetComponent<Box>()->shapeDraw = selected;

	}
}

void buki::EditorEntity::Start()
{
	RigidBody* rb = m_Entity->AddComponent<RigidBody>();
	rb->Type = RigidBody::BodyType::Static;
	Box* box = m_Entity->AddComponent<Box>();
	box->DebugColor = Color(0, 0, 255);
	box->Sensor = true;
	box->Collider.m_CanDraw = false;

	box->Collider.Size = (m_Entity->GetTransform()->GetSize() / 2);
	box->filter = 9;
	m_Entity->ActivatePhysics();
}

void buki::EditorEntity::Destroy()
{
}

json buki::EditorEntity::Serialize()
{
	return json();
}

void buki::EditorEntity::Deserialize(json _doc)
{
}

void buki::EditorEntity::Set()
{
}
