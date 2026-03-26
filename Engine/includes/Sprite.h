#pragma once
#include <Component.h>
#include <IDrawable.h>
#include <BukiContainers.h>
#include <Color.h>
#include <Flip.h>
#include <RectI.h>
#include <RectF.h>
#include <string>
#include <vector>
#include "Engine.h"

namespace buki
{
	//[SerializableComponent]
	struct Sprite : public Component, public IDrawable
	{
		virtual ~Sprite() = default;
		Sprite(Entity* _entity);

		virtual void Draw(float alpha) override;
		virtual void Load(const std::string& _path);

		virtual json Serialize() override;
		virtual void Deserialize(json _doc) override;
		virtual void Set() override;

		bool GetFlipH() const { return m_Flip.h; }
		bool GetFlipV() const { return m_Flip.v; }
		Color GetColor() const { return m_Color; }
		void SetColor(const Color& color);
		void SetFlip(bool h, bool v);

		inline Vector2 GetPosition() const { return positionOffset; }
		inline Vector2 GetSize() const { return sizeOffset; }
		inline int GetW() const { return static_cast<int>(sizeOffset.x); }
		inline int GetH() const { return static_cast<int>(sizeOffset.y); }
		virtual inline void SetPosition(Vector2 _position) { positionOffset = _position; }
		inline void SetSize(Vector2 _size) { sizeOffset = _size; }
		inline std::string GetPath() { return path; }
	protected:
		Flip m_Flip = Flip();
		std::string path;
		Color m_Color = Color::WHITE;
		Vector2 positionOffset = Vector2();
		Vector2 sizeOffset = Vector2();
		size_t m_Id = 0;
		RectI m_Src = RectI();
		RectF m_LastDst = RectF();
	};
}