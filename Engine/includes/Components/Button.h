#pragma once

#include "Component.h"
#include "IUpdatable.h"
#include "IDrawable.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Font2D.h"
#include "BukiContainers.h"

#include <functional>
#include <string>

namespace buki
{
	struct ButtonStyle
	{
		Color textColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		Color textHoverColor{ 1.0f, 1.0f, 0.6f, 1.0f };

		Color backgroundColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		Color backgroundHoverColor{ 1.0f, 1.0f, 1.0f, 1.0f };

		Color outlineColor{ 0.0f, 1.0f, 0.0f, 1.0f };
		Color outlineHoverColor{ 1.0f, 1.0f, 0.0f, 1.0f };

		Vector2 padding{ 0.25f, 0.15f };
		Vector2 textOffset{ 0.0f, 0.0f };

		bool showBackground = true;
		bool fitToText = true;
		bool drawOutline = false;
		bool drawOutlineOnHoverOnly = true;
		bool centerTextX = true;
		bool centerTextY = true;
	};

	class Button final : public Component, public IUpdatable, public IDrawable
	{
	public:
		explicit Button(Entity* entity);
		~Button() override = default;

		void Start() override;
		void Update(float dt) override;
		void Draw(float alpha) override;
		void Destroy() override;

		json Serialize() override;
		void Deserialize(json doc) override;
		void Set() override;

		void SetText(const std::string& text) { m_Text = text; }
		const std::string& GetText() const { return m_Text; }

		void SetFontPath(const std::string& path) { m_FontPath = path; }
		const std::string& GetFontPath() const { return m_FontPath; }

		void SetFontSize(int size) { m_FontSize = size; }
		int GetFontSize() const { return m_FontSize; }

		void SetBackgroundImage(const std::string& path) { m_BackgroundImagePath = path; }
		const std::string& GetBackgroundImage() const { return m_BackgroundImagePath; }

		void SetStyle(const ButtonStyle& style) { m_Style = style; }
		ButtonStyle& Style() { return m_Style; }
		const ButtonStyle& Style() const { return m_Style; }

		Vector2 GetPosition() const;
		Vector2 GetSize() const;
		Vector2 GetTextSize() const;
		RectF GetBounds() const;

		std::string GetMessage() const { return message; }	

		bool IsHovered() const { return m_Hovered; }
		bool IsPressed() const { return m_Pressed; }

		void SetOnClick(const std::function<void()>& callback) { m_OnClick = callback; }

		void SetMessage(const std::string& msg) { message = msg; }
	private:
		void RefreshResources();
		void RefreshLayout();
		bool ContainsPoint(const Vector2& point) const;

	private:
		std::string m_Text = "Button";
		std::string m_FontPath = "./fonts/Kenney/Kenney Blocks.ttf";
		std::string m_BackgroundImagePath;

		int m_FontSize = 24;

		Font2D* m_Font = nullptr;
		Texture2D* m_BackgroundTexture = nullptr;

		ButtonStyle m_Style{};

		bool m_Hovered = false;
		bool m_Pressed = false;
		bool m_ClickedLastFrame = false;

		std::function<void()> m_OnClick;
		std::string message;
	};
}