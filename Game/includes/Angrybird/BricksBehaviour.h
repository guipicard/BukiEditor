#pragma once
#include "MonoBehaviour.h"

namespace buki
{
	struct Sprite;
	struct BricksBehaviour : public MonoBehaviour
	{

		BricksBehaviour(Entity* _entity);
		~BricksBehaviour() = default;
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


		void AddImage(const std::string& image);
		void AddCollisionSound(const std::string& sound);
		void AddDamageSound(const std::string& sound);
		void AddBreakSound(const std::string& sound);
		void PlayCollisionSound();
		void PlayDamageSound();
		void PlayBreakSound();
		void SetMaxHealth(const float amount);
	private:
		void TakeDamage(float damage);
		std::vector<std::string> stageImages;
		std::vector<std::string> collisionSounds;
		std::vector<std::string> DamageSounds;
		std::vector<std::string> BreakSounds;
		std::map<std::string, size_t> soundMap;
		Sprite* spriteComponent;
		float maxHealth = 100.0f;
		float health;
	};
}
