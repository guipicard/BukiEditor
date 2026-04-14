#pragma once
#include "Entity.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "Engine.h"
#include "BukiContainers.h"
#include "Shapes.h"
#include "Sprite.h"
#include "Text.h"
#include "ComponentRegistration.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;
buki::Entity::Entity()
	: Entity("new Entity")
{
}

buki::Entity::Entity(std::string _name)
	: m_Name(_name)
{
	transform = new Transform();
	Initialize(Vector2(), 0.0f, Vector2(1.0f, 1.0f));
}

void buki::Entity::Start()
{
	for (auto it = m_ComponentByType.begin(); it != m_ComponentByType.end(); ++it)
	{
		it->second->Start();
	}
}

void buki::Entity::Draw(float alpha)
{
	if (!enabled) return;
	/*for (auto c : m_Drawable)
	{
		c->Draw(alpha);
	}*/
	/*for (size_t i = 0; i < m_Drawable.size(); i++)
	{
		m_Drawable[i]->Draw(alpha);
	}*/
	for (auto& drawable : m_Drawable) {
		drawable->Draw(alpha);
	}
}

void buki::Entity::FixedUpdate(float dt)
{
	if (!enabled) return;
	for (auto c : m_FixedUpdatable)
	{
		c->FixedUpdate(dt);
	}
}

void buki::Entity::Update(float dt)
{
	if (!enabled) return;
	for (auto c : m_Updatable)
	{
		c->Update(dt);
	}
}

void buki::Entity::Destroy()
{
	DeactivatePhysics();
	for (std::map<const type_info*, Component*>::iterator it = m_ComponentByType.begin(); it != m_ComponentByType.end(); ++it)
	{
		it->second->Destroy();
		delete it->second;
		it->second = nullptr;
	}
	m_ComponentByType.clear();
	m_Drawable.clear();
	m_Updatable.clear();
	delete transform;
	transform = nullptr;
}

void buki::Entity::Initialize(Vector2 position, float rotation, Vector2 size)
{
	transform->SetPosition(position);
	transform->SetRotation(rotation);
	transform->SetSize(size);
}

void buki::Entity::ActivatePhysics()
{
	if (m_Physics) return;
	RigidBody* rb = GetComponent<RigidBody>();
	if (!rb)
	{
		rb = AddComponent<RigidBody>();
	}
	rb->SetBodyId(Engine::Get().Physics().CreatePhysicsBody(this));
	m_Physics = true;

	Shapes* shape = GetComponentOfType<Shapes>();
	if (shape)
	{
		shape->SetPhysics();
	}
}

void buki::Entity::DeactivatePhysics()
{
	if (!m_Physics) return;
	RigidBody* rb = this->GetComponent<RigidBody>();
	if (rb)
	{
		Engine::Get().Physics().DestroyPhysicsBody(rb->GetBodyId());
		m_Physics = false;
	}
}

json buki::Entity::Serialize() const 
{
	json doc;
	doc["position"]["x"] = transform->GetPosition().x;
	doc["position"]["y"] = transform->GetPosition().y;
	doc["rotation"] = transform->GetRotation().GetRadians();
	doc["size"]["x"] = transform->GetSize().x;
	doc["size"]["y"] = transform->GetSize().y;
	doc["z"] = zAxis;
	doc["layer"] = layer;
	doc["physics"] = m_Physics;
	for each(std::pair<const type_info*, Component*> cmp in m_ComponentByType)
	{
		const std::type_info* typeInfo = cmp.first;
		Component* component = cmp.second;
		if (component->IsSerialized()) continue;
		std::string typeName = ComponentFactory::GetTypeName(*typeInfo);
		Engine::Get().Log().LogMessage("Serializing component: " + typeName);
		if (typeName.empty())
		{
			Engine::Get().Log().LogMessage("no string for this cmp type");
		}
		doc["components"][typeName] = cmp.second->Serialize();
		component->SetSerialized(true);
	}
	doc["enable"] = enabled;
	return doc;
}

void buki::Entity::Deserialize(json _doc)
{
	Vector2 position;
	position.x = _doc["position"]["x"].get<float>();
	position.y = _doc["position"]["y"].get<float>();
	float rotation = _doc["rotation"].get<float>();
	Vector2 size;
	size.x = _doc["size"]["x"].get<float>();
	size.y = _doc["size"]["y"].get<float>();
	Initialize(position, rotation, size);
	zAxis = _doc["z"].get<int>();
	layer = _doc["layer"].get<std::string>();
	if (_doc["physics"].get<bool>() && GetComponent<RigidBody>() == nullptr)
	{
		ComponentFactory::Create(this, "RigidBody", _doc["components"]["RigidBody"]);
	}
	enabled = _doc["enable"].get<bool>();
	for (auto& componentData : _doc["components"].items())
	{
		const std::string& typeName = componentData.key();
		const json& componentJson = componentData.value();
		if (typeName == "RigidBody") continue;
		ComponentFactory::Create(this, typeName, componentJson);
		
	}
	
	if (GetComponent<RigidBody>())
	{
		ActivatePhysics();
	}
}

void buki::Entity::Set()
{
}