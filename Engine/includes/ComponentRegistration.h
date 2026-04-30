#pragma once
#include "Engine.h"
#include "Polygon.h"
#include "Entity.h"
#include "Animation.h"
#include "Spawner.h"
#include "Box.h"
#include "Circle.h"
#include "Sprite.h"
#include "RigidBody.h"
#include "Text.h"
#include "Button.h"
#include "TileLayer.h"
#include "ComponentFactory.h"

template<typename T>
struct ComponentRegistrar {
	ComponentRegistrar(const std::string& name) {
		ComponentFactory::Register(name,
			[](buki::Entity* e, const json& j) {
				T* c = e->AddComponent<T>();
				c->Deserialize(j);
				//c->Set();
				return c;
			},
			typeid(T)
		);
	}
};

// One macro per component type
#define REGISTER_COMPONENT(TYPE, NAME_STRING) \
    namespace { ComponentRegistrar<TYPE> g_registrar_##TYPE(NAME_STRING); }

using namespace buki;
REGISTER_COMPONENT(TileLayer, "TileLayer");
REGISTER_COMPONENT(Text, "Text");
REGISTER_COMPONENT(Sprite, "Sprite");
REGISTER_COMPONENT(RigidBody, "RigidBody");
REGISTER_COMPONENT(Box, "Box");
REGISTER_COMPONENT(Polygon, "Polygon");
REGISTER_COMPONENT(Circle, "Circle");
REGISTER_COMPONENT(Button, "Button");
REGISTER_COMPONENT(Spawner, "Spawner");
REGISTER_COMPONENT(Animation, "Animation");

