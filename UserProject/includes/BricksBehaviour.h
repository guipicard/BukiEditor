#pragma once
#include "MonoBehaviour.h"

namespace buki
{
	class Sprite;
	struct BricksBehaviour : public MonoBehaviour
	{

		BricksBehaviour(Entity* _entity);
		~BricksBehaviour() = default;

		virtual void Start() override;
		virtual void Destroy() override;

		virtual void Update(const float dt) override;
		virtual void FixedUpdate(const float dt) override;

		virtual void OnCollisionEnter(Entity* other) override;
		virtual void OnCollisionExit(Entity* other) override;
		virtual void OnCollisionHit(Entity* other) override;
		virtual void OnSensorEnter(Entity* other) override;
		virtual void OnSensorExit(Entity* other) override;

		virtual void OnSet() override;


	public:
		float maxHealth = 100.0f;
		std::vector<std::string> stageImages;
		std::vector<std::string> collisionSounds;
		std::vector<std::string> DamageSounds;
		std::vector<std::string> BreakSounds;
			const std::vector<PropertyInfo>& GetProperties() const override
			{
				static std::vector<PropertyInfo> properties = {
					BUKI_PROP_FLOAT(BricksBehaviour, maxHealth),
					BUKI_PROP_IMAGE_LIST(BricksBehaviour, stageImages),
					BUKI_PROP_AUDIO_LIST(BricksBehaviour, collisionSounds),
					BUKI_PROP_AUDIO_LIST(BricksBehaviour, DamageSounds),
					BUKI_PROP_AUDIO_LIST(BricksBehaviour, BreakSounds),
				};
				return properties;
			}

	public:
		void AddImage(const std::string& image);
		void AddCollisionSound(const std::string& sound);
		void AddDamageSound(const std::string& sound);
		void AddBreakSound(const std::string& sound);
		void PlayCollisionSound();
		void PlayDamageSound();
		void PlayBreakSound();
	private:
		void TakeDamage(float damage);
		std::map<std::string, size_t> soundMap;
		Sprite* spriteComponent = nullptr;
		float health = 100.0f;
	};
}
