#pragma once
#include "BukiScene.h"

namespace buki
{
	struct Atlas;
	struct Entity;
	struct Platformer : public BukiScene
	{
		Platformer();
		virtual void CodeLoad() override;
		virtual void OnWindowResize() override;
	private:
		void GroundAndBackground();
		Entity* Ground;
		Atlas* GroundAtlas;
		Entity* Background;
		Atlas* BackgroundAtlas;
		Entity* PlayerEntity;
	};
}