#pragma once
#include "MonoBehaviour.h"

namespace buki
{
	struct RigidBody;
	struct Animation;
	class SpriteAnimator;
	struct Box;
	struct Circle;
	struct Polygon;
	struct AnimTest : public MonoBehaviour
	{
		AnimTest(Entity* entity);
		~AnimTest() = default;
		virtual void Awake() override;
		virtual void Start() override;
		virtual void FixedUpdate(const float dt) override;
		virtual void Destroy() override;
		virtual void Update(const float dt) override;
		virtual void OnCollisionEnter(Entity* other) override;
		virtual void OnCollisionExit(Entity* other) override;
		virtual void OnCollisionHit(Entity* other) override;
		virtual void OnSensorEnter(Entity* other) override;
		virtual void OnSensorExit(Entity* other) override;

		virtual json Serialize() override;
		virtual void Deserialize(json _doc) override;
		virtual void Set() override;

		//void Jump(float _force);
		//void Move(Vector2 _dir, float _maxVel, float _speed);
		Animation* GetAnimation() const { return anim; }
	private:
		Animation* anim = nullptr;
		Box* boxCollider = nullptr;
		Circle* circleCollider = nullptr;
		Polygon* polygonCollider = nullptr;
		RigidBody* rigidbody = nullptr;

		size_t jumpSFX = 0;

	};
}
