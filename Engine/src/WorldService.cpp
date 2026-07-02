#pragma once
#include "WorldService.h"
#include "Engine.h"
#include "Entity.h"
#include "RigidBody.h"
#include "MonoBehaviour.h"
#include "BukiScene.h"
#include "Text.h"
#include <algorithm>
#include <unordered_set>
#include "ComponentRegistration.h"
#include <fstream>
#include <exception>
#include "nlohmann/json.hpp"
#include <string>
#include <filesystem>

using json = nlohmann::json;
namespace fs = std::filesystem;

buki::WorldService::WorldService()
{
	Start();
}

void buki::WorldService::Start()
{

}

void buki::WorldService::FixedUpdate(const float dt)
{
	Engine::Get().Physics().Step(dt);
	for (auto entity : m_EntityInWorld)
	{
		entity->FixedUpdate(dt);
	}
}

void buki::WorldService::Update(float dt)
{
	for (int i = static_cast<int>(m_EntityInWorld.size()) - 1; i >= 0; i--)
	{
		m_EntityInWorld[i]->Update(dt);
	}
	CleanEntities();
	if (m_SceneToLoad != "")
	{
		Load(m_SceneToLoad);
		SortEntities();
	}
}

void buki::WorldService::Render(float alpha)
{
	for (auto entity : m_EntityInWorld)
	{
		entity->Draw(alpha);
	}
}

void buki::WorldService::Destroy()
{
	Unload();

	UnloadPrefabAssets();
	for (std::map<std::string, IScene*>::iterator it = m_Scenes.begin(); it != m_Scenes.end(); ++it)
	{
		if (it->second != nullptr)
		{
			delete it->second;
			it->second = nullptr;
		}
	}

	m_Scenes.clear();
	m_ScenesByName.clear();

	m_Name.clear();
	m_SceneToLoad.clear();
	m_CurrentScene = nullptr;
	m_CurrentScenePath.clear();
	m_SceneSource = SceneSource::None;
}

void buki::WorldService::Add(Entity* _entity)
{
	m_EntityMap.emplace(_entity->GetName(), _entity);
	m_EntityInWorld.push_back(_entity);
	SortEntities();
}

void buki::WorldService::Remove(Entity* _entity)
{
	if (std::find(m_EntityToRemove.begin(), m_EntityToRemove.end(), _entity) == m_EntityToRemove.end())
	{
		m_EntityToRemove.emplace_back(_entity);
	}
}

void buki::WorldService::RemoveFromScene(Entity* _entity)
{
	if (_entity == nullptr)
	{
		return;
	}
	Remove(_entity);
	CleanEntities();
}

buki::Entity* buki::WorldService::FindEntityByName(std::string _name)
{
	auto it = m_EntityMap.find(_name);
	if (it == m_EntityMap.end())
	{
		return nullptr;
	}
	return it->second;
}

std::vector<buki::Entity*> buki::WorldService::FindAll(std::string _name)
{
	std::vector<Entity*> list = std::vector<Entity*>();
	for (auto obj : m_EntityInWorld)
	{
		if (obj->GetName() == _name)
		{
			list.push_back(obj);
		}
	}
	return list;
}

void buki::WorldService::Load(const std::string& scene)
{
	Unload();
	if (m_Scenes.count(scene) > 0)
	{
		m_CurrentScene = m_Scenes[scene];
		SetCurrentSceneName(scene);
		m_CurrentScenePath.clear();
		m_SceneSource = SceneSource::Registered;
		m_CurrentScene->Load();
		m_CurrentScene->OnStart();
	}
	else
	{
		Engine::Get().Log().LogError("Scene " + scene + " not found, loading Menu scene instead.");
		m_CurrentScene = m_Scenes["Menu"];
		SetCurrentSceneName("Menu");
		m_CurrentScenePath.clear();
		m_SceneSource = SceneSource::Registered;
		m_CurrentScene->Load();
		m_CurrentScene->OnStart();
	}

	m_SceneToLoad.clear();
}

bool buki::WorldService::LoadScene(const std::string& path)
{
	fs::path scenePath = fs::absolute(fs::path("../Deployment") / path);
	std::ifstream file(scenePath);
	if (!file.is_open())
	{
		return false;
	}

	json doc;
	try
	{
		file >> doc;
	}
	catch (const std::exception&)
	{
		return false;
	}

	Unload();

	try
	{
		m_CurrentScene = nullptr;
		m_CurrentScenePath = path;
		m_SceneSource = SceneSource::File;

		if (doc.contains("sceneName") && doc["sceneName"].is_string())
		{
			SetCurrentSceneName(doc["sceneName"].get<std::string>());
		}
		else
		{
			SetCurrentSceneName(std::filesystem::path(path).stem().string());
		}

		if (doc.contains("camera") && doc["camera"].is_object())
		{
			Engine::Get().GetActiveCameraPtr()->Deserialize(doc["camera"]);
		}

		if (doc.contains("entities") && doc["entities"].is_object())
		{
			for (auto& [entityName, entityDoc] : doc["entities"].items())
			{
				Entity* entity = CreateEntity(entityName);
				if (entity != nullptr)
				{
					entity->Deserialize(entityDoc);
				}
			}
		}

		SortEntities();
		m_SceneToLoad.clear();
		return true;
	}
	catch (const std::exception&)
	{
		Unload();
		return false;
	}
}

bool buki::WorldService::SaveScene(const std::string& path) const
{
	json doc;

	const std::filesystem::path filePath(path);
	doc["sceneName"] = filePath.stem().string();
	doc["scenePath"] = path;
	doc["camera"] = Engine::Get().GetActiveCameraPtr()->Serialize();
	doc["entities"] = json::object();

	std::unordered_set<std::string> usedNames;

	for (Entity* entity : m_EntityInWorld)
	{
		if (entity == nullptr)
		{
			continue;
		}

		std::string entityName = entity->GetName();
		if (entityName.empty())
		{
			entityName = "Entity";
		}

		std::string uniqueName = entityName;
		int suffix = 1;
		while (usedNames.count(uniqueName) > 0)
		{
			uniqueName = entityName + "_" + std::to_string(suffix++);
		}
		usedNames.insert(uniqueName);

		doc["entities"][uniqueName] = entity->Serialize();
	}

	std::ofstream file(fs::absolute(fs::path("../Deployment") / path));
	if (!file.is_open())
	{
		return false;
	}

	file << doc.dump(4);
	return true;
}

void buki::WorldService::SetLoadScene(const std::string& scene)
{
	m_SceneToLoad = scene;
}

void buki::WorldService::Unload()
{
	CleanEntities();
	if (m_CurrentScene != nullptr)
	{
		m_CurrentScene->OnStop();
	}

	for (auto entity : m_EntityInWorld)
	{
		if (entity != nullptr)
		{
			entity->Destroy();
			delete entity;
			entity = nullptr;
		}
	}

	m_EntityMap.clear();
	m_EntityInWorld.clear();
	m_EntityToRemove.clear();

	m_CurrentScene = nullptr;
	m_CurrentScenePath.clear();
	m_SceneSource = SceneSource::None;
}

void buki::WorldService::Register(const std::string& name)
{
	if (m_Scenes.count(name) == 0)
	{
		m_ScenesByName.push_back(name);
		m_Scenes[name] = new BukiScene();
		BukiScene* bukiScene = dynamic_cast<BukiScene*>(m_Scenes[name]);
		if (bukiScene != nullptr)
		{
			bukiScene->SetName(name);
			bukiScene->Initialize();
		}
	}
}

buki::Entity* buki::WorldService::CreateEntity(const std::string& name)
{
	Entity* _e = new Entity(MakeUniqueEntityName(name));
	Add(_e);
	return _e;
}

void buki::WorldService::LoadNextScene()
{
	int currentIndex = -1;

	for (int i = 0; i < static_cast<int>(m_ScenesByName.size()); i++)
	{
		if (m_ScenesByName[i] == m_Name)
		{
			currentIndex = i;
			break;
		}
	}

	if (currentIndex == -1 || m_ScenesByName.empty())
	{
		SetLoadScene("Menu");
		return;
	}

	int nextIndex = currentIndex + 1;
	if (nextIndex >= static_cast<int>(m_ScenesByName.size()))
	{
		SetLoadScene("Menu");
		return;
	}

	SetLoadScene(m_ScenesByName[nextIndex]);
}

const std::vector<buki::Entity*>& buki::WorldService::GetEntitiesInWorld()
{
	return m_EntityInWorld;
}


void buki::WorldService::SortEntities()
{
	std::vector<int> layerList = std::vector<int>();
	std::map<int, std::vector<Entity*>> zMap = std::map<int, std::vector<Entity*>>();
	for (Entity* entity : m_EntityInWorld)
	{
		int z = entity->GetZ();
		zMap[z].push_back(entity);
		if (layerList.size() == 0 || std::find(layerList.begin(), layerList.end(), z) == layerList.end())
		{
			layerList.push_back(z);
		}
	}
	std::sort(layerList.begin(), layerList.end());
	m_EntityInWorld.clear();
	for (int z : layerList)
	{
		for (Entity* e : zMap[z])
		{
			m_EntityInWorld.push_back(e);
		}
	}
}

bool buki::WorldService::SaveCurrentScene() const
{
	if (m_CurrentScenePath.empty())
	{
		return false;
	}
	return SaveScene(m_CurrentScenePath);
}

bool buki::WorldService::RenameEntity(Entity* entity, const std::string& newName)
{
	if (entity == nullptr || newName.empty())
	{
		return false;
	}

	const std::string oldName = entity->GetName();
	if (oldName == newName)
	{
		return true;
	}

	auto existing = m_EntityMap.find(newName);
	if (existing != m_EntityMap.end() && existing->second != entity)
	{
		return false;
	}

	auto it = m_EntityMap.find(oldName);
	if (it != m_EntityMap.end() && it->second == entity)
	{
		m_EntityMap.erase(it);
	}

	entity->SetName(newName);
	m_EntityMap[newName] = entity;
	return true;
}

std::string buki::WorldService::MakeUniqueEntityName(const std::string& baseName) const
{
	if (baseName.empty())
	{
		return "Entity";
	}

	std::string candidate = baseName;
	int index = 1;
	while (buki::Engine::Get().GetWorldPtr()->FindEntityByName(candidate) != nullptr)
	{
		candidate = baseName + " (" + std::to_string(index) + ")";
		++index;
	}

	return candidate;
}

buki::Entity* buki::WorldService::InstantiatePrefab(const std::string& prefabPath)
{
	if (prefabPath.empty())
	{
		return nullptr;
	}
	fs::path path;
	if (prefabPath[0] == '.')
	{
		path = prefabPath;
	}
	else
	{
		path = fs::path(("../Deployment/" + prefabPath));
	}
	Entity* prefabEntity = GetOrLoadPrefabEntity(prefabPath);
	if (prefabEntity == nullptr)
	{
		return nullptr;
	}

	Entity* entity = ClonePrefabEntity(prefabEntity);
	if (entity == nullptr)
	{
		return nullptr;
	}

	const std::string baseName = path.stem().string();
	const std::string uniqueName = MakeUniqueEntityName(baseName);

	entity->SetName(uniqueName);
	Add(entity);
	return entity;
}

buki::Entity* buki::WorldService::ClonePrefabEntity(Entity* source)
{
	if (source == nullptr)
	{
		return nullptr;
	}

	Entity* clone = new Entity();
	clone->Deserialize(source->Serialize());
	return clone;
}

std::unordered_map<std::string, buki::PrefabAssetInstance>& buki::WorldService::GetPrefabAssets()
{
	return prefabAssets;
}

buki::Entity* buki::WorldService::GetOrLoadPrefabEntity(const std::string& path)
{
	if (path.empty())
	{
		return nullptr;
	}

	const fs::path absolutePath = fs::absolute("../Deployment/" +  path).lexically_normal();
	const std::string key = absolutePath.string();

	std::error_code ec;
	const bool exists = fs::exists(absolutePath, ec);
	if (ec || !exists)
	{
		return nullptr;
	}

	const auto writeTime = fs::last_write_time(absolutePath, ec);
	if (ec)
	{
		return nullptr;
	}

	auto it = prefabAssets.find(key);
	if (it != prefabAssets.end())
	{
		PrefabAssetInstance& asset = it->second;

		if (asset.entity != nullptr && asset.lastWriteTime == writeTime && !asset.dirty)
		{
			return asset.entity;
		}

		if (!asset.dirty)
		{
			if (asset.entity != nullptr)
			{
				delete asset.entity;
				asset.entity = nullptr;
			}
		}
		else
		{
			return asset.entity;
		}
	}

	std::ifstream in(absolutePath);
	if (!in.is_open())
	{
		return nullptr;
	}

	json doc;
	try
	{
		in >> doc;
	}
	catch (const std::exception&)
	{
		return nullptr;
	}

	Entity* entity = new Entity(MakeUniqueEntityName(absolutePath.stem().string()));
	entity->Deserialize(doc);
	entity->Set();

	PrefabAssetInstance& slot = prefabAssets[key];
	slot.path = path;
	slot.entity = entity;
	slot.lastWriteTime = writeTime;
	slot.dirty = false;
	return entity;
}

bool buki::WorldService::SavePrefabAsset(const std::string& path)
{
	const fs::path absolutePath = fs::absolute("../Deployment/" + path).lexically_normal();
	const std::string key = absolutePath.string();

	auto it = prefabAssets.find(key);
	if (it == prefabAssets.end() || it->second.entity == nullptr)
	{
		return false;
	}

	std::error_code ec;
	fs::create_directories(absolutePath.parent_path(), ec);

	std::ofstream out(absolutePath);
	if (!out.is_open())
	{
		return false;
	}

	out << it->second.entity->Serialize().dump(4);
	out.close();

	it->second.path = path;
	it->second.lastWriteTime = fs::last_write_time(absolutePath, ec);
	it->second.dirty = false;
	return true;
}

void buki::WorldService::UnloadPrefabAssets()
{
	for (auto& [key, asset] : prefabAssets)
	{
		if (asset.entity != nullptr)
		{
			delete asset.entity;
			asset.entity = nullptr;
		}
	}
	prefabAssets.clear();
}

void buki::WorldService::CleanEntities()
{
	if (m_EntityToRemove.size() > 0)
	{
		std::vector<Entity*> _trash = m_EntityToRemove;

		for (auto entity : _trash)
		{
			std::map<std::string, Entity*>::iterator it = m_EntityMap.begin();
			while (it != m_EntityMap.end())
			{
				if (it->second == entity)
				{
					m_EntityMap.erase(it);
					break;
				}
				it++;
			}

			for (auto it = m_EntityInWorld.begin(); it != m_EntityInWorld.end(); it++)
			{
				if (entity == *it)
				{
					m_EntityInWorld.erase(it);
					break;
				}
			}

			entity->Destroy();
			delete entity;
		}
		_trash.clear();
		m_EntityToRemove.clear();
	}
}

