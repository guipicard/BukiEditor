#pragma once
#include "BukiScene.h"

namespace buki
{
	class Entity;
	class TileLayer;
	struct Platformer : public BukiScene
	{
		Platformer();
		virtual void CodeLoad() override;
		virtual void OnWindowResize() override;
		virtual void OnNotify(const std::string& button) override;
	private:
		void GroundAndBackground();
		Entity* Ground;
		TileLayer* GroundAtlas;
		Entity* Background;
		TileLayer* BackgroundAtlas;
		Entity* PlayerEntity;
	};
}