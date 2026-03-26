#pragma once
#include "WorldService.h"
#include "Engine.h"
#include "Entity.h"
#include "BaseScene.h"
#include "Collider.h"
#include "BoxCollider.h"
#include "RigidBody.h"
#include "CircleCollider.h"
#include "MonoBehaviour.h"
#include "BukiScene.h"
#include "Text.h"
#include <algorithm>
#include <unordered_set>
#include "ComponentRegistration.h"


buki::WorldService::WorldService()
{
	Start();
}

void buki::WorldService::Start()
{

}

void buki::WorldService::FixedUpdate(float dt)
{
	Engine::GetInstance().Physics().Step(dt);
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
	for (std::map<std::string, IScene*>::iterator it = m_Scenes.begin(); it != m_Scenes.end(); ++it)
	{
		if (it->second != nullptr)
		{
			delete it->second;
			it->second = nullptr;
		}
	}
	m_Name = "";
	m_SceneToLoad = "";
	m_ScenesByName.clear();
	m_CurrentScene = nullptr;
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

buki::Entity* buki::WorldService::Find(std::string _name)
{
	return m_EntityMap[_name];
}

std::vector<buki::Entity*> buki::WorldService::FindAll(std::string _name)
{
	std::vector<Entity*> list = std::vector<Entity*>();
	for each(auto obj in m_EntityInWorld)
	{
		if (obj->GetName() == _name)
		{
			list.push_back(obj);
		}
	}
	return list;
}

void buki::WorldService::Load(const std::string& scene) {
	if (m_Scenes.count(scene) > 0)
	{
		Unload();
		m_CurrentScene = m_Scenes[scene];
		SetCurrentSceneName(scene);
		m_CurrentScene->Load();
		m_CurrentScene->OnStart();
	}
	else
	{
		Unload();
		m_CurrentScene = m_Scenes["Menu"];
		SetCurrentSceneName("Menu");
		m_CurrentScene->Load();
		m_CurrentScene->OnStart();
	}

	m_SceneToLoad = "";
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
		Engine::GetInstance().Physics().Reset();
	}
}

void buki::WorldService::Register(const std::string& name, IScene* scene)
{
	if (m_Scenes.count(name) == 0)
	{
		m_ScenesByName.push_back(name);
		m_Scenes[name] = scene;
		BukiScene* bukiScene = dynamic_cast<BukiScene*>(scene);
		if (bukiScene != nullptr)
		{
			bukiScene->SetName(name);
			bukiScene->Initialize();
		}
	}
}

buki::Entity* buki::WorldService::Create(const std::string& name)
{
	Entity* _e = new Entity(name);
	Add(_e);
	return _e;
}

void buki::WorldService::LoadNextScene()
{
	int index = -1;
	for (int i = 0; i < m_ScenesByName.size(); i++)
	{
		if (m_ScenesByName[i] == m_Name)
		{
			index = i + 1;

		}
	}
	if (index > m_ScenesByName.size() - 1)
	{
		SetLoadScene("Menu");
	}
	if (index != -1) SetLoadScene(m_ScenesByName[index]);
}

void buki::WorldService::SortEntities()
{
	std::vector<int> layerList = std::vector<int>();
	std::map<int, std::vector<Entity*>> zMap = std::map<int, std::vector<Entity*>>();
	for each(Entity * entity in m_EntityInWorld)
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
