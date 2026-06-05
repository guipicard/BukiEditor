#pragma once
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <string>

namespace buki
{
	class Entity;
	class IScene;
	
	struct PrefabAssetInstance
	{
		std::filesystem::path path;
		Entity* entity = nullptr;
		std::filesystem::file_time_type lastWriteTime{};
		bool dirty = false;
	};

	class IWorld
	{
	public:
		virtual ~IWorld() = default; // Destructeur virtuel par défaut.

		virtual void Start() = 0; // Initialise le monde du jeu.

		virtual void FixedUpdate(float dt) = 0;

		virtual void Update(float dt) = 0; // Met à jour le monde du jeu, avec dt représentant le temps écoulé depuis la dernière mise à jour.

		virtual void Render(float alpha) = 0; // Dessine le monde du jeu à l'écran.

		virtual void Destroy() = 0; // Nettoie et libère les ressources utilisées par le monde du jeu.

		virtual Entity* CreateEntity(const std::string& name) = 0; // Crée une nouvelle entité avec un nom spécifié.

		virtual void Add(Entity* _entity) = 0; // Ajoute une entité au monde du jeu.

		virtual void Remove(Entity* _entity) = 0; // Supprime une entité du monde du jeu.

		virtual void RemoveFromScene(Entity* _entity) = 0;

		virtual Entity* FindEntityByName(std::string _name) = 0; // Recherche une entité dans le monde du jeu.

		virtual std::vector<Entity*> FindAll(std::string _name) = 0; // Recherche des entités au meme nom dans le monde du jeu.

		virtual void Load(const std::string& scene) = 0; // Charge une scène spécifiée par son nom.

		virtual bool LoadScene(const std::string& path) = 0;

		virtual bool SaveScene(const std::string& path) const = 0;

		virtual void SetLoadScene(const std::string& scene) = 0; // Définit la scène à charger.

		virtual void Unload() = 0; // Décharge la scène actuelle.

		virtual void Register(const std::string& name) = 0; // Enregistre une scène avec un nom spécifique dans le monde du jeu.

		virtual void SetCurrentSceneName(std::string _name) = 0; // Définit le nom de la scène actuelle.

		virtual IScene* GetCurrentScene() = 0; // Retourne la scène actuelle.
		
		virtual std::string GetCurrentSceneName() = 0; // Retourne le nom de la scène actuelle.

		virtual void LoadNextScene() = 0; // Charge la scène suivante dans la séquence définie.

		virtual const std::vector<Entity*>& GetEntitiesInWorld() = 0; // Get All enties present in the world

		virtual void SortEntities() = 0; // sorts Entities present in the World by Z axis

		virtual const std::string& GetCurrentScenePath() const = 0; // Retourne le chemin de la scène actuelle.
		virtual bool HasCurrentScenePath() const = 0;
		virtual bool SaveCurrentScene() const = 0;
		virtual bool RenameEntity(Entity* entity, const std::string& newName) = 0;
		virtual std::string MakeUniqueEntityName(const std::string& baseName) const = 0;

		virtual Entity* GetOrLoadPrefabEntity(const std::filesystem::path& path) = 0;
		virtual Entity* InstantiatePrefab(const std::string& prefabPath) = 0;
		virtual Entity* ClonePrefabEntity(Entity* source) = 0;
		virtual bool SavePrefabAsset(const std::filesystem::path& path) = 0;
		virtual void UnloadPrefabAssets() = 0;
		virtual std::unordered_map<std::string, PrefabAssetInstance>& GetPrefabAssets() = 0;
	};
}
