#include "GameComponentRegistration.h"

#include "ComponentRegistration.h"

#include "AngryBirdController.h"
#include "BricksBehaviour.h"
#include "ShapesController.h"
#include "AnimTest.h"
#include "OpenGlTest.h"

void game::RegisterGameComponents()
{
	REGISTER_COMPONENT_EXPLICIT(buki::AngryBirdController, "AngryBirdController");
	REGISTER_COMPONENT_EXPLICIT(buki::BricksBehaviour, "BricksBehaviour");
	REGISTER_COMPONENT_EXPLICIT(buki::ShapesController, "ShapesController");
	REGISTER_COMPONENT_EXPLICIT(buki::AnimTest, "AnimTest");
	REGISTER_COMPONENT_EXPLICIT(buki::OpenGlTest, "OpenGlTest");
}