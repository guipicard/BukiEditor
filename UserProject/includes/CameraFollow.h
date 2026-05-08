#pragma once
#include "MonoBehaviour.h"
#include "EntityRef.h"

namespace buki
{
	class CameraFollow : public MonoBehaviour
	{
	public:
		CameraFollow(Entity* entity);
		CameraFollow() = default;

		virtual void Start() override;
		virtual void Destroy() override;
		virtual void Update(float dt) override;
		virtual void FixedUpdate(float dt) override;
		virtual void OnCollisionEnter(Entity* other) override;
		virtual void OnCollisionExit(Entity* other) override;
		virtual void OnCollisionHit(Entity* other) override;
		virtual void OnSensorEnter(Entity* other) override;
		virtual void OnSensorExit(Entity* other) override;
		virtual void OnSet() override;

		const std::vector<PropertyInfo>& GetProperties() const override
		{
			static std::vector<PropertyInfo> properties = {
				BUKI_PROP_ENTITY(CameraFollow, target)
			};
			return properties;
		}

	public:
		EntityRef target;
	};
}