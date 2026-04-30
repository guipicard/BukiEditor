#pragma once
#include "Core/IScene.h"

namespace buki
{
	class Entity;
	struct BukiScene : public IScene
	{
	public:
		BukiScene();
		~BukiScene();
		virtual Entity* Instantiate(const std::string _name) override;
		virtual void OnStart() override;
		virtual void OnStop() override;
		virtual void OnNotify(const std::string& button) = 0;
		virtual void Load() override;
		void SetName(const std::string& _name) { name = _name; }
		std::string GetName() const { return name; }
		void Initialize();
		void SaveScene() const;
		void SaveScene(std::string fileName) const;
		virtual void CodeLoad() = 0;
		void FileLoad(std::string _path) const;
		void SetScene() override;
	protected:
		std::string name;
	};
}
