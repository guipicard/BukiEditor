#include "EngineComponentRegistration.h"

#include "ComponentRegistration.h"

#include "Polygon.h"
#include "Animation.h"
#include "Spawner.h"
#include "Box.h"
#include "Circle.h"
#include "Sprite.h"
#include "RigidBody.h"
#include "Text.h"
#include "Button.h"
#include "TileLayer.h"

void buki::RegisterEngineComponents()
{
	REGISTER_COMPONENT_EXPLICIT(TileLayer, "TileLayer");
	REGISTER_COMPONENT_EXPLICIT(Text, "Text");
	REGISTER_COMPONENT_EXPLICIT(Sprite, "Sprite");
	REGISTER_COMPONENT_EXPLICIT(RigidBody, "RigidBody");
	REGISTER_COMPONENT_EXPLICIT(Box, "Box");
	REGISTER_COMPONENT_EXPLICIT(Polygon, "Polygon");
	REGISTER_COMPONENT_EXPLICIT(Circle, "Circle");
	REGISTER_COMPONENT_EXPLICIT(Button, "Button");
	REGISTER_COMPONENT_EXPLICIT(Animation, "Animation");
}