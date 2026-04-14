#pragma once
#include "Engine.h"
#include "ComponentFactory.h"
#include "Entity.h"

#include "Animation.h"
#include "Spawner.h"
#include "Box.h"
#include "Circle.h"
#include "Sprite.h"
#include "Atlas.h"
#include "RigidBody.h"
#include "Text.h"
#include "Button.h"


template<typename T>
struct ComponentRegistrar {
	ComponentRegistrar(const std::string& name) {
		ComponentFactory::Register(name,
			[](buki::Entity* e, const json& j) {
				T* c = e->GetComponent<T>();
				if (c)
				{
					if (c->IsDeserialized())
					{
						Engine::Get().Log().LogWarning("Component of type " + std::string(typeid(T).name()) + " already exists on entity " + e->GetName() + " and is marked as deserialized. Skipping deserialization.");
						return; // THIS MAKES IT SO AN ENTITY CANT HAVE 2 COMPONENTS OF THE SAME TYPE, WHICH IS GOOD FOR NOW, BUT MAYBE NOT LATER
					}
				}
				else
				{
					c = e->AddComponent<T>();
					if (c->IsSavable())
					{
						c->Deserialize(j);
						c->Set();
					}
				}
				c->SetDeserialized(true);
			},
			typeid(T)
		);
	}
};

// One macro per component type
#define REGISTER_COMPONENT(TYPE, NAME_STRING) \
    namespace { ComponentRegistrar<TYPE> g_registrar_##TYPE(NAME_STRING); }

using namespace buki;
//namespace buki

REGISTER_COMPONENT(Animation, "Animation");
//REGISTER_COMPONENT(Atlas, "Atlas");
REGISTER_COMPONENT(Box, "Box");
REGISTER_COMPONENT(Button, "Button");
REGISTER_COMPONENT(Circle, "Circle");
REGISTER_COMPONENT(RigidBody, "RigidBody");
REGISTER_COMPONENT(Sprite, "Sprite");
REGISTER_COMPONENT(Text, "Text");
REGISTER_COMPONENT(Spawner, "Spawner");

