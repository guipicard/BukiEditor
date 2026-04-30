#pragma once
#include "BukiScene.h"
#include "Entity.h"
#include "Subject.h"
#include "Observer.h"
#include "Button.h"
#include "Component.h"
#include <string>

namespace buki
{
	struct Entity;
	struct MenuScene : public BukiScene
	{
		MenuScene();
		virtual void CodeLoad() override;
		virtual void OnWindowResize() override;
		virtual void OnNotify(const std::string& button) override;
	};
}
