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

	struct AngryBirdDemo : public BukiScene, Observer<std::string>
	{
		AngryBirdDemo();
		virtual void CodeLoad() override;
		virtual void OnWindowResize() override;
		virtual void OnNotify(const std::string& button) override;
	private:
		void CreateMenu();
		Spawner* spawner;
	};
}