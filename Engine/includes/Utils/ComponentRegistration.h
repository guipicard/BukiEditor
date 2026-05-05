#pragma once

#include "ComponentFactory.h"
#include "Entity.h"

#define REGISTER_COMPONENT_EXPLICIT(TYPE, NAME_STRING) \
	buki::ComponentFactory::Register( \
		NAME_STRING, \
		[](buki::Entity* e, const json& j) -> buki::Component* \
		{ \
			TYPE* c = e->AddComponent<TYPE>(); \
			if (j != json{}) c->Deserialize(j); \
			return c; \
		}, \
		typeid(TYPE) \
	)