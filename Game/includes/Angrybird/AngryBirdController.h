#pragma once
#include "MonoBehaviour.h"

namespace buki
{
	struct Spawner;
	struct Button;
	struct AngryBirdController : public MonoBehaviour
	{
		AngryBirdController(Entity* _entity);
		~AngryBirdController() = default;

		virtual void Awake() override;
		virtual void Start() override;
		virtual void Destroy() override;
		virtual void FixedUpdate(const float dt) override;
		virtual void Update(const float dt) override;
		virtual void OnCollisionEnter(Entity* other) override;
		virtual void OnCollisionExit(Entity* other) override;
		virtual void OnCollisionHit(Entity* other) override;
		virtual void OnSensorEnter(Entity* other) override;
		virtual void OnSensorExit(Entity* other) override;

		virtual json Serialize() override;	
		virtual void Deserialize(json _doc) override;
		virtual void Set() override;

		void AddCollisionSound(const std::string& sound);
		void PlayCollisionSound();
	private:
		Entity* anchor = nullptr;
		Vector2 anchorPos;
		Vector2 clickPos;
		void Throw(const Vector2 _v);
		void Reset();
		bool aiming = false;
		bool thrown = false;
		float forceMultiplier;
		float shotLength;
		size_t aimingSound;
		size_t launchingSound;
		std::vector<std::size_t> collisionSounds;
		float soundDamageThreshold = 5.0f;

		void EditorController();
		Vector2 firstMousePos = { 0.0f,0.0f };

		int debugUpdateCounter = 0;

		std::vector<Button*> buttonsInScene;
		bool UIHovered = false;
	};
}