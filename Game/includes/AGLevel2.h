#pragma once
#include "BukiScene.h"
#include "BukiContainers.h"
#include <cstdlib>
#include <map>
#include <vector>
#include "Observer.h"

namespace buki
{
	class Entity;
	struct Sprite;
	struct Spawner;
	class TileLayer;

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
		TileLayer* GroundSprite;
		Entity* Background;
		TileLayer* BackgroundImage;
		Spawner* spawner;
		Entity* slignshot1;
		Entity* slignshot2;
		std::string currentLevel = "";
	};
}