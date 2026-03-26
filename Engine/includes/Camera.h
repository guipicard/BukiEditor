#pragma once
#include "RectI.h"
#include "BukiContainers.h"
#include "nlohmann/json.hpp"

namespace buki
{
	using json = nlohmann::json;
	struct Camera
	{
		Camera();
		~Camera();
		Vector2 GetPosition() const;
		void SetPosition(const Vector2 pos);
		json Serialize();
		void Deserialize(json _doc);
		int width = 0;
		int height = 0;
		float zoomScale = 1.0f;
		Vector2 position = Vector2::ZERO;
		
	};
}
