#pragma once
#include "BukiScene.h"
#include "BukiContainers.h"
#include <cstdlib>
#include <map>
#include <vector>
#include "Observer.h"

namespace buki
{
	struct Entity;
	struct Sprite;
	struct Atlas;
	struct Spawner;

	struct AGLevel2 : public BukiScene, Observer<std::string>
	{
		AGLevel2();
		virtual void CodeLoad() override;
		virtual void OnWindowResize() override;
		virtual void OnNotify(const std::string& button) override;
	private:
		void GroundAndBackground();
		void InitLevel();
		void SpawnSlignshot(float _offset);
		void CreateLevelMenu();
		Entity* Ground;
		Atlas* GroundSprite;
		Entity* Background;
		Atlas* BackgroundImage;
		Spawner* spawner;
		Entity* slignshot1;
		Entity* slignshot2;
		std::string currentLevel = "";
	};
}