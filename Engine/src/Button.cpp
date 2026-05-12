#pragma once
#include "Button.h"

#include "BukiContainers.h"
#include "Texture2D.h"
#include "Engine.h"
#include "Entity.h"
#include "BukiScene.h"

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
	}

	void Button::Set()
	{
		RefreshResources();
		RefreshLayout();
		SetOnClick([this]()
		{
			BukiScene* currentScene = dynamic_cast<BukiScene*>(Engine::Get().World().GetCurrentScene());
			if (currentScene)
			{
				currentScene->OnNotify(message);
			}
		});
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
		if (m_Entity == nullptr || m_Entity->T() == nullptr)
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

			m_Entity->T()->SetSize(finalSize);
		}
	}

	Vector2 Button::GetPosition() const
	{
		if (m_Entity == nullptr || m_Entity->T() == nullptr)
		{
			return Vector2{};
		}

		return m_Entity->T()->GetPosition();
	}

	Vector2 Button::GetSize() const
	{
		if (m_Entity == nullptr || m_Entity->T() == nullptr)
		{
			return Vector2{};
		}

		return m_Entity->T()->GetSize();
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

		bool pressedNow = m_Hovered && Input().IsMouseButtonDown(0);
		m_Pressed = pressedNow;

		if (pressedNow && !m_ClickedLastFrame && m_OnClick)
		{
			m_OnClick();
		}

		m_ClickedLastFrame = pressedNow;
	}

	void Button::Draw(float alpha)
	{
		(void)alpha;

		if (m_Entity == nullptr || m_Entity->T() == nullptr)
		{
			return;
		}

		const Vector2 pos = m_Entity->T()->GetPosition();
		const Vector2 size = m_Entity->T()->GetSize();
		float rotation = m_Entity->T()->GetRotation().GetRadians();

		const Color bgColor = m_Hovered ? m_Style.backgroundHoverColor : m_Style.backgroundColor;
		const Color textColor = m_Hovered ? m_Style.textHoverColor : m_Style.textColor;
		const Color outlineColor = m_Hovered ? m_Style.outlineHoverColor : m_Style.outlineColor;

		if (m_Style.showBackground)
		{
			if (m_BackgroundTexture != nullptr)
			{
				RectF source = RectF{
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
					m_Entity->T()->GetRotation().GetRadians(),
					false,
					false,
					bgColor
				);
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
				m_Style.centerTextY
			);
		}

		const bool drawOutline =
			m_Style.drawOutline &&
			(!m_Style.drawOutlineOnHoverOnly || m_Hovered);

		if (drawOutline)
		{
			Graphics().DrawRectOutline(pos, size, 0.0f, outlineColor);
		}
	}

	json Button::Serialize()
	{
		json doc;

		doc["text"] = m_Text;
		doc["fontPath"] = m_FontPath;
		doc["fontSize"] = m_FontSize;
		doc["backgroundImagePath"] = m_BackgroundImagePath;

		doc["style"]["textColor"]["r"] = m_Style.textColor.r;
		doc["style"]["textColor"]["g"] = m_Style.textColor.g;
		doc["style"]["textColor"]["b"] = m_Style.textColor.b;
		doc["style"]["textColor"]["a"] = m_Style.textColor.a;

		doc["style"]["textHoverColor"]["r"] = m_Style.textHoverColor.r;
		doc["style"]["textHoverColor"]["g"] = m_Style.textHoverColor.g;
		doc["style"]["textHoverColor"]["b"] = m_Style.textHoverColor.b;
		doc["style"]["textHoverColor"]["a"] = m_Style.textHoverColor.a;

		doc["style"]["backgroundColor"]["r"] = m_Style.backgroundColor.r;
		doc["style"]["backgroundColor"]["g"] = m_Style.backgroundColor.g;
		doc["style"]["backgroundColor"]["b"] = m_Style.backgroundColor.b;
		doc["style"]["backgroundColor"]["a"] = m_Style.backgroundColor.a;

		doc["style"]["backgroundHoverColor"]["r"] = m_Style.backgroundHoverColor.r;
		doc["style"]["backgroundHoverColor"]["g"] = m_Style.backgroundHoverColor.g;
		doc["style"]["backgroundHoverColor"]["b"] = m_Style.backgroundHoverColor.b;
		doc["style"]["backgroundHoverColor"]["a"] = m_Style.backgroundHoverColor.a;

		doc["style"]["outlineColor"]["r"] = m_Style.outlineColor.r;
		doc["style"]["outlineColor"]["g"] = m_Style.outlineColor.g;
		doc["style"]["outlineColor"]["b"] = m_Style.outlineColor.b;
		doc["style"]["outlineColor"]["a"] = m_Style.outlineColor.a;

		doc["style"]["outlineHoverColor"]["r"] = m_Style.outlineHoverColor.r;
		doc["style"]["outlineHoverColor"]["g"] = m_Style.outlineHoverColor.g;
		doc["style"]["outlineHoverColor"]["b"] = m_Style.outlineHoverColor.b;
		doc["style"]["outlineHoverColor"]["a"] = m_Style.outlineHoverColor.a;

		doc["style"]["padding"]["x"] = m_Style.padding.x;
		doc["style"]["padding"]["y"] = m_Style.padding.y;

		doc["style"]["textOffset"]["x"] = m_Style.textOffset.x;
		doc["style"]["textOffset"]["y"] = m_Style.textOffset.y;

		doc["style"]["showBackground"] = m_Style.showBackground;
		doc["style"]["fitToText"] = m_Style.fitToText;
		doc["style"]["drawOutline"] = m_Style.drawOutline;
		doc["style"]["drawOutlineOnHoverOnly"] = m_Style.drawOutlineOnHoverOnly;
		doc["style"]["centerTextX"] = m_Style.centerTextX;
		doc["style"]["centerTextY"] = m_Style.centerTextY;

		doc["message"] = message;

		return doc;
	}

	void Button::Deserialize(json doc)
	{
		m_Text = doc.value("text", std::string("Button"));
		m_FontPath = doc.value("fontPath", std::string("./fonts/Kenney/Kenney Blocks.ttf"));
		m_FontSize = doc.value("fontSize", 24);
		m_BackgroundImagePath = doc.value("backgroundImagePath", std::string(""));

		if (doc.contains("style"))
		{
			const json& style = doc["style"];

			if (style.contains("textColor"))
			{
				m_Style.textColor.r = style["textColor"].value("r", m_Style.textColor.r);
				m_Style.textColor.g = style["textColor"].value("g", m_Style.textColor.g);
				m_Style.textColor.b = style["textColor"].value("b", m_Style.textColor.b);
				m_Style.textColor.a = style["textColor"].value("a", m_Style.textColor.a);
			}

			if (style.contains("textHoverColor"))
			{
				m_Style.textHoverColor.r = style["textHoverColor"].value("r", m_Style.textHoverColor.r);
				m_Style.textHoverColor.g = style["textHoverColor"].value("g", m_Style.textHoverColor.g);
				m_Style.textHoverColor.b = style["textHoverColor"].value("b", m_Style.textHoverColor.b);
				m_Style.textHoverColor.a = style["textHoverColor"].value("a", m_Style.textHoverColor.a);
			}

			if (style.contains("backgroundColor"))
			{
				m_Style.backgroundColor.r = style["backgroundColor"].value("r", m_Style.backgroundColor.r);
				m_Style.backgroundColor.g = style["backgroundColor"].value("g", m_Style.backgroundColor.g);
				m_Style.backgroundColor.b = style["backgroundColor"].value("b", m_Style.backgroundColor.b);
				m_Style.backgroundColor.a = style["backgroundColor"].value("a", m_Style.backgroundColor.a);
			}

			if (style.contains("backgroundHoverColor"))
			{
				m_Style.backgroundHoverColor.r = style["backgroundHoverColor"].value("r", m_Style.backgroundHoverColor.r);
				m_Style.backgroundHoverColor.g = style["backgroundHoverColor"].value("g", m_Style.backgroundHoverColor.g);
				m_Style.backgroundHoverColor.b = style["backgroundHoverColor"].value("b", m_Style.backgroundHoverColor.b);
				m_Style.backgroundHoverColor.a = style["backgroundHoverColor"].value("a", m_Style.backgroundHoverColor.a);
			}

			if (style.contains("outlineColor"))
			{
				m_Style.outlineColor.r = style["outlineColor"].value("r", m_Style.outlineColor.r);
				m_Style.outlineColor.g = style["outlineColor"].value("g", m_Style.outlineColor.g);
				m_Style.outlineColor.b = style["outlineColor"].value("b", m_Style.outlineColor.b);
				m_Style.outlineColor.a = style["outlineColor"].value("a", m_Style.outlineColor.a);
			}

			if (style.contains("outlineHoverColor"))
			{
				m_Style.outlineHoverColor.r = style["outlineHoverColor"].value("r", m_Style.outlineHoverColor.r);
				m_Style.outlineHoverColor.g = style["outlineHoverColor"].value("g", m_Style.outlineHoverColor.g);
				m_Style.outlineHoverColor.b = style["outlineHoverColor"].value("b", m_Style.outlineHoverColor.b);
				m_Style.outlineHoverColor.a = style["outlineHoverColor"].value("a", m_Style.outlineHoverColor.a);
			}

			if (style.contains("padding"))
			{
				m_Style.padding.x = style["padding"].value("x", m_Style.padding.x);
				m_Style.padding.y = style["padding"].value("y", m_Style.padding.y);
			}

			if (style.contains("textOffset"))
			{
				m_Style.textOffset.x = style["textOffset"].value("x", m_Style.textOffset.x);
				m_Style.textOffset.y = style["textOffset"].value("y", m_Style.textOffset.y);
			}

			m_Style.showBackground = style.value("showBackground", m_Style.showBackground);
			m_Style.fitToText = style.value("fitToText", m_Style.fitToText);
			m_Style.drawOutline = style.value("drawOutline", m_Style.drawOutline);
			m_Style.drawOutlineOnHoverOnly = style.value("drawOutlineOnHoverOnly", m_Style.drawOutlineOnHoverOnly);
			m_Style.centerTextX = style.value("centerTextX", m_Style.centerTextX);
			m_Style.centerTextY = style.value("centerTextY", m_Style.centerTextY);

			message = doc.value("message", std::string(""));
		}
	}
}