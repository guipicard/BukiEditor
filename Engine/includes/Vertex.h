#pragma once
#include "BukiContainers.h"
#include "Color.h"

namespace buki
{
	struct Vertex
	{
		Vertex() : Vertex(Vector2(), Color(), Vector2()) {}
		Vertex(Vector2 Positon, Color Color, Vector2 Size) : vertexPosition(Positon), vertexColor(Color), vertexSize(Size) {}
		void GetData(Vector2& position, Color& color, Vector2& size) const { position = vertexPosition; color = vertexColor; size = vertexSize; }
	private:
		Vector2 vertexPosition;
		Color vertexColor;
		Vector2 vertexSize;
	};
}
