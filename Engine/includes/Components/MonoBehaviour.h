#pragma once

#include "Component.h"
#include "IUpdatable.h"
#include "IFixedUpdatable.h"
#include "PropertyInfo.h"
#include "nlohmann/json.hpp"
#include <vector>

namespace buki
{
	class MonoBehaviour : public Component, public IUpdatable, public IFixedUpdatable
	{
	public:
		MonoBehaviour(Entity* entity);
		virtual ~MonoBehaviour() = default;

		virtual const std::vector<PropertyInfo>& GetProperties() const
		{
			static std::vector<PropertyInfo> empty;
			return empty;
		}

		virtual void OnInspectorGUI()
		{
		}

		virtual void Update(float dt) = 0;
		virtual void FixedUpdate(float dt) = 0;

		virtual json Serialize() override;
		virtual void Deserialize(json doc) override;
		virtual void Set() = 0;

		// MONOBEHAVIOUR
		virtual void OnCollisionEnter(Entity* other) = 0;
		virtual void OnCollisionExit(Entity* other) = 0;
		virtual void OnCollisionHit(Entity* other) = 0;
		virtual void OnSensorEnter(Entity* other) = 0;
		virtual void OnSensorExit(Entity* other) = 0;
	};
}
