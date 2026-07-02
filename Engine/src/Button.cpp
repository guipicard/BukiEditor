#pragma once
#include "Button.h"

#include "BukiContainers.h"
#include "Texture2D.h"
#include "Engine.h"
#include "Entity.h"
#include "ScriptFunctionRegistry.h"

namespace buki
{
	Button::Button(Entity* entity)
		: Component(entity)
	{
		if (m_Entity != nullptr)
		{
			m_Entity->SetZ(10);
		}
	}

	void Button::Start()
	{
	}

	void Button::Destroy()
	{
		m_OnClick = nullptr;
		m_PressedInside = false;
	}

	void Button::Set()
	{
		RefreshResources();
		RefreshLayout();
		ResolveOnClickBinding();
	}

	void Button::RefreshResources()
	{
		m_Font = nullptr;
		m_BackgroundTexture = nullptr;

		if (!m_FontPath.empty() && m_FontSize > 0)
		{
			m_Font = Fonts().Load(m_FontPath, m_FontSize);
		}

		if (!m_BackgroundImagePath.empty())
		{
			m_BackgroundTexture = Textures().Load(m_BackgroundImagePath);
		}
	}

	Vector2 Button::GetTextSize() const
	{
		if (m_Font == nullptr || !m_Font->IsValid())
		{
			return Vector2{ 0.0f, 0.0f };
		}

		return Graphics().MeasureText(*m_Font, m_Text);
	}

	void Button::RefreshLayout()
	{
		if (m_Entity == nullptr)
		{
			return;
		}

		if (m_Style.fitToText)
		{
			const Vector2 textSize = GetTextSize();
			const Vector2 finalSize =
			{
				textSize.x + m_Style.padding.x * 2.0f,
				textSize.y + m_Style.padding.y * 2.0f
			};

			m_Entity->Tm().SetSize(finalSize);
		}
	}

	void Button::ResolveOnClickBinding()
	{
		m_OnClick = nullptr;

		if (m_OnClickBinding.IsEmpty())
		{
			return;
		}

		const StaticVoidFunctionInfo* fn =
			ScriptFunctionRegistry::Get().FindStaticVoidFunction(
				m_OnClickBinding.scriptTypeName,
				m_OnClickBinding.functionName);

		if (fn == nullptr)
		{
			return;
		}

		m_OnClick = fn->callback;
	}

	Vector2 Button::GetPosition() const
	{
		if (m_Entity == nullptr)
		{
			return Vector2{};
		}

		return m_Entity->Tm().GetPosition();
	}

	Vector2 Button::GetSize() const
	{
		if (m_Entity == nullptr)
		{
			return Vector2{};
		}

		return m_Entity->Tm().GetSize();
	}

	RectF Button::GetBounds() const
	{
		const Vector2 pos = GetPosition();
		const Vector2 size = GetSize();

		return RectF
		{
			pos.x - size.x * 0.5f,
			pos.y - size.y * 0.5f,
			size.x,
			size.y
		};
	}

	bool Button::ContainsPoint(const Vector2& point) const
	{
		const RectF bounds = GetBounds();

		return point.x >= bounds.x &&
			point.x <= bounds.x + bounds.w &&
			point.y >= bounds.y &&
			point.y <= bounds.y + bounds.h;
	}

	void Button::Update(float dt)
	{
		(void)dt;

		Vector2 mousePos{ 0.0f, 0.0f };
		Input().GetMousePositionWorld(&mousePos.x, &mousePos.y);

		m_Hovered = ContainsPoint(mousePos);

		const bool mousePressedThisFrame = Input().IsMouseButtonPressed(0);
		const bool mouseReleasedThisFrame = Input().IsMouseButtonUp(0);

		if (mousePressedThisFrame && m_Hovered)
		{
			m_PressedInside = true;
		}

		if (mouseReleasedThisFrame)
		{
			const bool shouldClick = m_PressedInside && m_Hovered && static_cast<bool>(m_OnClick);
			m_PressedInside = false;

			if (shouldClick)
			{
				m_OnClick();
			}
		}
	}

	void Button::Draw(float alpha)
	{
		(void)alpha;

		if (m_Entity == nullptr )
		{
			return;
		}

		const auto& t = m_Entity->Tm();
		const Vector2 pos = t.GetPosition();
		const Vector2 size = t.GetSize();

		const Color bgColor = m_Hovered ? m_Style.backgroundHoverColor : m_Style.backgroundColor;
		const Color textColor = m_Hovered ? m_Style.textHoverColor : m_Style.textColor;
		const Color outlineColor = m_Hovered ? m_Style.outlineHoverColor : m_Style.outlineColor;

		if (m_Style.showBackground)
		{
			if (m_BackgroundTexture != nullptr)
			{
				RectF source =
				{
					0.0f,
					0.0f,
					static_cast<float>(m_BackgroundTexture->width),
					static_cast<float>(m_BackgroundTexture->height)
				};

				Graphics().DrawSprite(
					*m_BackgroundTexture,
					Camera(),
					glm::vec2{ pos.x, pos.y },
					size.x,
					size.y,
					source,
					t.GetRotation().GetRadians(),
					false,
					false,
					bgColor);
			}
			else
			{
				Graphics().FillRect(pos, size, 0.0f, bgColor);
			}
		}

		if (m_Font != nullptr && m_Font->IsValid())
		{
			const Vector2 textPos = pos + m_Style.textOffset;

			Graphics().DrawTextToCamera(
				*m_Font,
				m_Text,
				Camera(),
				glm::vec2{ textPos.x, textPos.y },
				textColor,
				m_Style.centerTextX,
				m_Style.centerTextY);
		}

		const bool drawOutline =
			m_Style.drawOutline &&
			(!m_Style.drawOutlineOnHoverOnly || m_Hovered);

		if (drawOutline)
		{
			Graphics().DrawRectOutline(pos, size, 0.0f, outlineColor);
		}
	}
}