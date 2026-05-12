#pragma once
//#include "BaseScene.h"
#include "BukiScene.h"
#include "Subject.h"
#include "Observer.h"

using namespace buki;

namespace buki
{
	class Entity;
	struct Spawner;
	struct Sprite;
	struct ShapesController;
	class TileLayer;
	struct ShapesDemo : public BukiScene
	{
		ShapesDemo();
		virtual void CodeLoad() override;
		virtual void OnWindowResize() override;
		virtual void OnNotify(const std::string& button) override;
	private:
		void GroundAndBackground();
		Entity* Ground;
		TileLayer* GroundSprite;
		Entity* Background;
		TileLayer* BackgroundImage;
	};
}
