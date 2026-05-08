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
		const std::vector<PropertyInfo>& GetProperties() const override
		{
			static std::vector<PropertyInfo> properties = {
				BUKI_PROP_FLOAT(AngryBirdController, shotLength),
				BUKI_PROP_FLOAT(AngryBirdController, forceMultiplier),
				BUKI_PROP_FLOAT(AngryBirdController, soundDamageThreshold)
				//BUKI_PROP_STRING(AngryBirdController, aimingSoundPath),
				//BUKI_PROP_STRING(AngryBirdController, launchingSoundPath),
			};
			return properties;
		}

		float shotLength = 4.0f;
		float forceMultiplier = 6.0f;
		float soundDamageThreshold = 5.0f;

		std::string aimingSoundPath = "";
		std::string launchingSoundPath = "";
		std::vector<std::string> collisionSoundPaths;

	public:
		void AddCollisionSound(const std::string& sound);
		void PlayCollisionSound();
	
	private:
		Entity* anchor = nullptr;
		Vector2 anchorPos = { 0.0f,0.0f };;
		void Throw(const Vector2 _v);
		void Reset();

		bool aiming = false;
		bool thrown = false;
		Vector2 firstMousePos = { 0.0f,0.0f };;
		Vector2 clickPos = { 0.0f,0.0f };

		size_t aimingSoundHandle = 0;
		size_t launchingSoundHandle = 0;
		std::vector<std::size_t> collisionSounds;


		std::vector<Button*> buttonsInScene;
	};
}