#pragma once

#include "Component.h"
#include "IUpdatable.h"
#include "IFixedUpdatable.h"
#include "nlohmann/json.hpp"
#include "ScriptFunctionRegistry.h"

namespace buki
{
	class MonoBehaviour : public Component, public IUpdatable, public IFixedUpdatable
	{
	public:
		MonoBehaviour(Entity* entity);
		virtual ~MonoBehaviour() = default;

		virtual void Update(float dt) = 0;
		virtual void FixedUpdate(float dt) = 0;

		virtual void Set() override;
		virtual void OnSet() = 0;

		// MONOBEHAVIOUR
		virtual void OnCollisionEnter(Entity* other) = 0;
		virtual void OnCollisionExit(Entity* other) = 0;
		virtual void OnCollisionHit(Entity* other) = 0;
		virtual void OnSensorEnter(Entity* other) = 0;
		virtual void OnSensorExit(Entity* other) = 0;
	};
}
