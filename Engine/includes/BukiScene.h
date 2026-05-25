#pragma once
#include "Core/IScene.h"

namespace buki
{
	class Entity;
	struct BukiScene : public IScene
	{
	public:
		BukiScene() = default;
		~BukiScene() = default;
		virtual void OnStart() override;
		virtual void OnStop() override;
		virtual bool Load() override;
		void Initialize();
		void SaveScene() const;
		void SaveScene(std::string fileName) const;
		void FileLoad(std::string _path) const;
		void SetScene() override;

		virtual void OnWindowResize() override;

		void SetName(const std::string& _name) { name = _name; }
		std::string GetName() const { return name; }
	protected:
		std::string name;
	};
}
