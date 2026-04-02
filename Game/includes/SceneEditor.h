#pragma once
#include "BukiScene.h"
#include "Observer.h"

namespace buki
{
	struct Entity;
	struct Sprite;
	struct Atlas;
	struct Spawner;

	struct SceneEditor : public BukiScene, Observer<std::string>
	{
		SceneEditor();
		virtual void CodeLoad() override;
		virtual void OnWindowResize() override;
		virtual void OnNotify(const std::string& button) override;
		virtual void OnStop() override;
	private:
		void EditorLoadScene(std::string scene);
		void TestImgui();
	};
}
