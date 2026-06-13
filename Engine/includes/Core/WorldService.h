#pragma once
#include "IWorld.h"
#include <fstream>
#include <filesystem>

namespace buki
{
	enum class SceneSource
	{
		None,
		Registered,
		File
	};
	
	struct WorldService : public IWorld
	{
		WorldService();
		virtual void Start() override;
		virtual void FixedUpdate(const float dt) override;
		virtual void Update(float dt) override;
		virtual void Render(float alpha) override;
		virtual void Destroy() override;
		virtual void Add(Entity* _entity) override;
		virtual void Remove(Entity* _entity) override;
		virtual void RemoveFromScene(Entity* _entity) override;
		virtual Entity* FindEntityByName(std::string _name) override;
		virtual std::vector<Entity*> FindAll(std::string _name) override;
		virtual void Load(const std::string& scene) override;
		virtual bool LoadScene(const std::string& path) override;
		virtual bool SaveScene(const std::string& path) const override;
		virtual void SetLoadScene(const std::string& scene) override;
		virtual void Unload() override;
		virtual void Register(const std::string& name) override;
		virtual IScene* GetCurrentScene() override { return m_CurrentScene; }
		virtual void SetCurrentSceneName(std::string _name) override { m_Name = _name; }
		virtual std::string GetCurrentSceneName() override { return m_Name; }
		virtual Entity* CreateEntity(const std::string& name) override;
		virtual void LoadNextScene() override;
		virtual const std::vector<Entity*>& GetEntitiesInWorld() override;
		virtual void SortEntities() override;
		virtual const std::string& GetCurrentScenePath() const override { return m_CurrentScenePath; }
		virtual bool HasCurrentScenePath() const override { return !m_CurrentScenePath.empty(); }
		virtual bool SaveCurrentScene() const override;
		virtual bool RenameEntity(Entity* entity, const std::string& newName) override;
		std::string MakeUniqueEntityName(const std::string& baseName) const;

		virtual Entity* InstantiatePrefab(const std::string& path) override;
		virtual Entity* ClonePrefabEntity(Entity* source) override;
		virtual std::unordered_map<std::string, PrefabAssetInstance>& GetPrefabAssets() override;
		virtual Entity* GetOrLoadPrefabEntity(const std::string& path) override;
		virtual bool SavePrefabAsset(const std::string& path) override;
		virtual void UnloadPrefabAssets() override;
	private:
		void CleanEntities();
		std::string m_Name;
		std::string m_SceneToLoad;
		std::map<std::string, Entity*> m_EntityMap;
		std::vector<Entity*> m_EntityInWorld;
		std::vector<Entity*> m_EntityToRemove;
		std::map<std::string, IScene*> m_Scenes;
		std::vector<std::string> m_ScenesByName;
		IScene* m_CurrentScene = nullptr;
		SceneSource m_SceneSource = SceneSource::None;
		std::string m_CurrentScenePath = "";
		std::unordered_map<std::string, PrefabAssetInstance> prefabAssets;
	};
}
