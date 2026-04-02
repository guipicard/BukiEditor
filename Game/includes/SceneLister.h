#pragma once
#include "MonoBehaviour.h"

namespace buki
{
	struct SceneLister : public buki::MonoBehaviour
	{
		SceneLister(buki::Entity* entity);
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Update(float dt) override;
		virtual void FixedUpdate(float dt) override;
		virtual void Destroy() override;
		virtual void OnCollisionEnter(Entity* other) override;
		virtual void OnCollisionExit(Entity* other) override;
		virtual void OnCollisionHit(Entity* other) override;
		virtual json Serialize() override;
		virtual void Deserialize(json _doc) override;
		virtual void Set() override;
	private:
		std::string scenePath = "./Scenes";
	};
}