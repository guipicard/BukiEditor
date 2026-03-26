#pragma once

#include <Sprite.h>
#include <IDrawable.h>
#include <Color.h>
#include <string>

namespace buki
{
	//[SerializableComponent]
	struct Text : public Sprite
	{
		Text(Entity* _entity);
		~Text();
		void Draw(float alpha);
		void LoadText(const std::string& _path, int size);
		std::string& GetText() const;
		inline void SetBackground(const bool _state) { m_Background = _state; }
		inline Vector2 GetSize() const { return textSize; }
		void SetText(std::string _text);
		//inline void GetSize(Vector2 _pos) { textPos = _pos; }
		inline void SetBackgroundColor(Color _color) { backgroundColor = _color; }
		virtual json Serialize() override;
		virtual void Deserialize(json _doc) override;
		virtual void Set() override;
		inline bool GetFixed() const { return fixed; }
		void SetFixed(const bool state);
		const Vector2 SetBackgroundOffset() const { return backgroundOffset; }
		const void SetBackgroundOffset(Vector2 offset) { backgroundOffset = offset; }
	private:
		void SetTextScalable(std::string _text);
		void SetTextFixed(std::string _text);
		std::string m_Text = "";
		bool m_Background = false;
		Vector2 textSize;
		Vector2 backgroundOffset;
		//Vector2 textPos;
		Color backgroundColor;
		int fontSize = 24;
		bool fixed = false;
	};
}
