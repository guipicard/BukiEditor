#pragma once
#include "IWorld.h"
namespace buki
{
	struct WorldService : public IWorld
	{
		WorldService();
		virtual void Start() override;
		virtual void FixedUpdate(float dt) override;
		virtual void Update(float dt) override;
		virtual void Render(float alpha) override;
		virtual void Destroy() override;
		virtual void Add(Entity* _entity) override;
		virtual void Remove(Entity* _entity) override;
		virtual Entity* Find(std::string _name) override;
		virtual std::vector<Entity*> FindAll(std::string _name) override;
		virtual void Load(const std::string& scene) override;
		virtual void SetLoadScene(const std::string& scene) override;
		virtual void Unload() override;
		virtual void Register(const std::string& name, IScene* scene) override;
		virtual IScene* GetCurrentScene() override { return m_CurrentScene; }
		virtual void SetCurrentSceneName(std::string _name) override { m_Name = _name; }
		virtual std::string GetCurrentSceneName() override { return m_Name; }
		virtual Entity* Create(const std::string& name) override;
		virtual void LoadNextScene() override;
		virtual std::vector<Entity*> GetEntitiesInWorld() override { return m_EntityInWorld; }
		virtual void SortEntities() override;
		
	private:
		void CleanEntities();
		std::string m_Name;
		std::string m_SceneToLoad;
		//std::map<std::string, Entity*> &m_EntityMap = *new std::map<std::string, Entity*>();
		//std::vector<Entity*> &m_EntityInWorld = *new std::vector<Entity*>();
		//std::vector<Entity*> &m_EntityToRemove = *new std::vector<Entity*>();
		//std::map<std::string, IScene*> &m_Scenes = *new std::map<std::string, IScene*>();
		//std::vector<std::string> &m_ScenesByName = *new std::vector<std::string>();
		std::map<std::string, Entity*> m_EntityMap;
		std::vector<Entity*> m_EntityInWorld;
		std::vector<Entity*> m_EntityToRemove;
		std::map<std::string, IScene*> m_Scenes;
		std::vector<std::string> m_ScenesByName;
		IScene* m_CurrentScene = nullptr;
	};
}
