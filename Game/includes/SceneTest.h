#pragma once
#include "BukiScene.h"
#include <cstdlib>
#include "Observer.h"
#include <string>

namespace buki
{
	class Entity;
	struct Sprite;
	struct Atlas;
	struct Spawner;

	struct SceneTest : public BukiScene, Observer<std::string>
	{
		SceneTest();
		virtual void CodeLoad() override;
		virtual void OnWindowResize() override;
		virtual void OnNotify(const std::string& button) override;
	};
}
