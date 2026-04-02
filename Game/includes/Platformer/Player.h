#pragma once
#include "MonoBehaviour.h"

namespace buki
{
	struct Sprite;
	struct RigidBody;
	struct Animation;
	struct Box;
	struct PlayerState;
	struct Text;
	struct Player : public MonoBehaviour
	{
		Player(Entity* entity);
		~Player() = default;
		void SetState(const std::string& state);
		virtual void Awake() override;
		virtual void Start() override;
		virtual void FixedUpdate(const float dt) override;
		virtual void Destroy() override;
		virtual void Update(const float dt) override;
		virtual void OnCollisionEnter(Entity* other) override;
		virtual void OnCollisionExit(Entity* other) override;
		virtual void OnCollisionHit(Entity* other) override;

		virtual json Serialize() override;
		virtual void Deserialize(json _doc) override;
		virtual void Set() override;

		void Jump(float _force);
		void Move(Vector2 _dir, float _maxVel, float _speed);

		Text* GetDebugText() { return debugText; }
	private:
		void UpdateDebugText();
		PlayerState* currentState;
		std::map<std::string, PlayerState*> states;
		Animation* playerAnim;
		Box* collider;
		RigidBody* rigidbody;
		Vector2 playerSize;
		Text* debugText;
	};
}
