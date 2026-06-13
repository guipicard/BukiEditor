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
	transform = MakeScope<Transform>();
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
	transform.reset();
}

buki::Transform& buki::Entity::T() const
{
	return *transform;
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
	auto t = T();
	doc["position"]["x"] = t.GetPosition().x;
	doc["position"]["y"] = t.GetPosition().y;
	doc["rotation"] = t.GetRotation().GetRadians();
	doc["size"]["x"] = t.GetSize().x;
	doc["size"]["y"] = t.GetSize().y;

	doc["z"] = zAxis;
	doc["layer"] = layer;
	doc["physics"] = m_Physics;
	doc["enable"] = enabled;

	for (const auto& [typeInfo, component] : m_ComponentByType)
	{
		std::string typeName = buki::ComponentFactory::GetTypeName(*typeInfo);
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
	Rot r = Rot();
	r.SetRadians(rotation);
	transform->UpdateState(position, r, size);

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

			Component* component = ComponentFactory::CreateCmp(this, typeName, componentJson);
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
		if (this != nullptr && component != nullptr)
		{
			component->Set();
		}
	}
}

bool buki::Entity::CanRemoveComponent(const std::string& typeName) const
{
	if (typeName.empty())
	{
		return false;
	}

	// Optional protected components
	if (typeName == "Transform")
	{
		return false;
	}

	return true;
}

bool buki::Entity::RemoveComponentByTypeName(const std::string& typeName)
{
	if (!CanRemoveComponent(typeName))
	{
		return false;
	}

	Component* target = nullptr;
	const std::type_info* targetType = nullptr;

	for (auto it = m_ComponentByType.begin(); it != m_ComponentByType.end(); ++it)
	{
		if (it->second == nullptr)
		{
			continue;
		}

		const std::string registeredName = ComponentFactory::GetTypeName(*it->first);
		if (registeredName == typeName)
		{
			target = it->second;
			targetType = it->first;
			break;
		}
	}

	if (target == nullptr || targetType == nullptr)
	{
		return false;
	}

	const bool hadPhysics = m_Physics;
	if (hadPhysics)
	{
		DeactivatePhysics();
	}

	if (dynamic_cast<IDrawable*>(target))
	{
		m_Drawable.erase(std::remove(m_Drawable.begin(), m_Drawable.end(), dynamic_cast<IDrawable*>(target)), m_Drawable.end());
	}
	if (dynamic_cast<IUpdatable*>(target))
	{
		m_Updatable.erase(std::remove(m_Updatable.begin(), m_Updatable.end(), dynamic_cast<IUpdatable*>(target)), m_Updatable.end());
	}
	if (dynamic_cast<IFixedUpdatable*>(target))
	{
		m_FixedUpdatable.erase(std::remove(m_FixedUpdatable.begin(), m_FixedUpdatable.end(), dynamic_cast<IFixedUpdatable*>(target)), m_FixedUpdatable.end());
	}

	target->Destroy();
	delete target;

	m_ComponentByType.erase(targetType);

	if (hadPhysics && GetComponent<RigidBody>() != nullptr)
	{
		ActivatePhysics();
	}

	return true;
}