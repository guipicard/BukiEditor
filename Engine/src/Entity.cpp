#pragma once
#include "Entity.h"
#include "RigidBody.h"
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
	if (m_Physics)
	{
		return;
	}

	RigidBody* rb = GetComponent<RigidBody>();
	if (rb == nullptr)
	{
		rb = AddComponent<RigidBody>();
	}

	const BodyId bodyId = Engine::Get().Physics().CreatePhysicsBody(this);
	rb->SetBodyId(bodyId);

	std::vector<Shapes*> shapes = GetAllComponentsOfType<Shapes>();
	if (shapes.empty())
	{
		Engine::Get().Log().LogMessage("Entity " + m_Name + " has no shapes to activate physics with.");
	}
	else
	{
		m_Physics = true;
		for (Shapes* shape : shapes)
		{
			if (shape != nullptr)
			{
				shape->SetPhysics();
			}
		}
	}
}

void buki::Entity::DeactivatePhysics()
{
	if (!m_Physics)
	{
		return;
	}

	RigidBody* rb = GetComponent<RigidBody>();
	if (rb == nullptr)
	{
		m_Physics = false;
		return;
	}

	Engine::Get().Physics().DestroyPhysicsBody(rb->GetBodyId());

	const std::vector<Shapes*> shapes = GetAllComponentsOfType<Shapes>();
	for (Shapes* shape : shapes)
	{
		if (shape != nullptr)
		{
			shape->SetShapeId({});
		}
	}

	m_Physics = false;
}
json buki::Entity::Serialize() const
{
	json doc;

	if (transform != nullptr)
	{
		doc["position"]["x"] = transform->GetPosition().x;
		doc["position"]["y"] = transform->GetPosition().y;
		doc["rotation"] = transform->GetRotation().GetRadians();
		doc["size"]["x"] = transform->GetSize().x;
		doc["size"]["y"] = transform->GetSize().y;
	}
	else
	{
		doc["position"]["x"] = 0.0f;
		doc["position"]["y"] = 0.0f;
		doc["rotation"] = 0.0f;
		doc["size"]["x"] = 0.0f;
		doc["size"]["y"] = 0.0f;
	}

	doc["z"] = zAxis;
	doc["layer"] = layer;
	doc["physics"] = m_Physics;
	doc["enable"] = enabled;

	for (const auto& [typeInfo, component] : m_ComponentByType)
	{
		std::string typeName = ComponentFactory::GetTypeName(*typeInfo);
		if (!typeName.empty() && component != nullptr)
		{
			doc["components"][typeName] = component->Serialize();
		}
	}

	return doc;
}

void buki::Entity::Deserialize(json _doc)
{
	Vector2 position{ 0.0f, 0.0f };
	Vector2 size{ 0.0f, 0.0f };
	float rotation = 0.0f;

	if (_doc.contains("position"))
	{
		position.x = _doc["position"].value("x", 0.0f);
		position.y = _doc["position"].value("y", 0.0f);
	}

	rotation = _doc.value("rotation", 0.0f);

	if (_doc.contains("size"))
	{
		size.x = _doc["size"].value("x", 0.0f);
		size.y = _doc["size"].value("y", 0.0f);
	}

	Initialize(position, rotation, size);

	zAxis = _doc.value("z", 0);
	layer = _doc.value("layer", std::string{});
	enabled = _doc.value("enable", true);
	m_Physics = false;

	if (_doc.contains("components") && _doc["components"].is_object())
	{
		for (auto& componentData : _doc["components"].items())
		{
			const std::string& typeName = componentData.key();
			const json& componentJson = componentData.value();

			Component* component = ComponentFactory::Create(this, typeName, componentJson);
			if (component == nullptr)
			{
				Engine::Get().Log().LogMessage("Unknown component type during deserialize: " + typeName);
			}
		}
	}
}

void buki::Entity::Set()
{
	for (const auto& [typeInfo, component] : m_ComponentByType)
	{
		if (component != nullptr)
		{
			component->Set();
		}
	}
}