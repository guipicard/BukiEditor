#pragma once
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

		const auto& t = m_Entity->Tm();
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