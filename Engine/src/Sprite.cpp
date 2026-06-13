#include "Sprite.h"

#include "Engine.h"
#include "Entity.h"
#include "Shapes.h"
#include "Box.h"
#include "Circle.h"
#include <cstdlib>
#include <string>
#include <cctype>
#include <algorithm>

namespace buki
{
	Sprite::Sprite(Entity* entity)
		: Component(entity)
	{
	}

	void Sprite::Draw(float alpha)
	{
		if (m_Texture == nullptr || !m_Texture->IsValid())
		{
			return;
		}

		const auto& t = m_Entity->T();
		const Vector2 pos = t.GetPosition() + m_PositionOffset;

		RectF source = m_SourceRectPixels;
		if (!m_UseSourceRect)
		{
			source = RectF{
				0.0f,
				0.0f,
				static_cast<float>(m_Texture->width),
				static_cast<float>(m_Texture->height)
			};
		}

		Graphics().DrawSprite(
			*m_Texture,
			Camera(),
			glm::vec2{ pos.x, pos.y },
			m_ImageSize.x,
			m_ImageSize.y,
			source,
			t.GetRotation().GetRadians(),
			m_Flip.h,
			m_Flip.v,
			m_Color
		);
	}

	json Sprite::Serialize()
	{
		json doc;
		doc["path"] = m_Path;
		doc["positionOffset"]["x"] = m_PositionOffset.x;
		doc["positionOffset"]["y"] = m_PositionOffset.y;
		doc["size"]["x"] = m_ImageSize.x;
		doc["size"]["y"] = m_ImageSize.y;
		doc["color"]["r"] = m_Color.r;
		doc["color"]["g"] = m_Color.g;
		doc["color"]["b"] = m_Color.b;
		doc["color"]["a"] = m_Color.a;
		doc["flip"]["h"] = m_Flip.h;
		doc["flip"]["v"] = m_Flip.v;
		doc["useSourceRect"] = m_UseSourceRect;
		doc["sourceRect"]["x"] = m_SourceRectPixels.x;
		doc["sourceRect"]["y"] = m_SourceRectPixels.y;
		doc["sourceRect"]["w"] = m_SourceRectPixels.w;
		doc["sourceRect"]["h"] = m_SourceRectPixels.h;
		return doc;
	}

	void Sprite::Deserialize(json doc)
	{
		m_Path = doc.value("path", "");
		m_PositionOffset.x = doc["positionOffset"]["x"];
		m_PositionOffset.y = doc["positionOffset"]["y"];
		Vector2 size{ 0.0f, 0.0f };
		size.x = doc["size"]["x"];
		size.y = doc["size"]["y"];
		SetImageSize(size);

		m_Color.r = doc["color"].value("r", 1.0f);
		m_Color.g = doc["color"].value("g", 1.0f);
		m_Color.b = doc["color"].value("b", 1.0f);
		m_Color.a = doc["color"].value("a", 1.0f);

		m_Flip.h = doc["flip"].value("h", false);
		m_Flip.v = doc["flip"].value("v", false);

		m_UseSourceRect = doc.value("useSourceRect", false);
		m_SourceRectPixels.x = doc["sourceRect"].value("x", 0.0f);
		m_SourceRectPixels.y = doc["sourceRect"].value("y", 0.0f);
		m_SourceRectPixels.w = doc["sourceRect"].value("w", 0.0f);
		m_SourceRectPixels.h = doc["sourceRect"].value("h", 0.0f);
	}

	void Sprite::Set()
	{
		m_Texture = nullptr;

		if (!m_Path.empty())
		{
			std::string deCap = m_Path;
			std::transform(deCap.begin(), deCap.end(), deCap.begin(), ::tolower);
			m_Texture = Textures().Load(m_Path, deCap.find("deployment"));
		}
	}

	void Sprite::SetPath(const std::string& path)
	{
		m_Path = path;
	}
	void Sprite::SetImageSize(Vector2 value)
	{
		m_ImageSize = value; 
	}
}