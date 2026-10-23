#pragma once

#include "Component.h"
#include "IDrawable.h"
#include "IUpdatable.h"
#include "PropertyInfo.h"
#include "AnimationStateMachine.h"
#include "AnimationTypes.h"
#include "AnimationAsset.h"

#include <string>
#include <vector>

namespace buki
{
	class Entity;

	class SpriteAnimator
	{
	public:
		SpriteAnimator() = default;
		~SpriteAnimator() = default;

		void Play(const AnimationClip* clip);
		void Stop();
		void Update(float dt);
		void Draw(float alpha);

		[[nodiscard]] const SpriteFrame* GetCurrentFrame() const;
		[[nodiscard]] bool IsPlaying() const { return m_Playing; }
		[[nodiscard]] std::string GetCurrentClipName() const { return m_CurrentClipName; }

	private:
		void AdvanceFrame(float dt);

		const AnimationClip* m_CurrentClip = nullptr;
		std::size_t m_FrameIndex = 0;
		float m_TimeRemaining = 0.0f;
		bool m_Playing = false;
		std::string m_CurrentClipName;
	};

	struct Animator : public Component, public IDrawable, public IUpdatable
	{
		Animator(Entity* entity);

		virtual void Start() override;
		virtual void Destroy() override;
		virtual void Draw(float alpha) override;
		virtual void Update(float dt) override;
		virtual void Set() override;

		void SetAnimationAsset(AnimationAsset* asset);
		[[nodiscard]] const AnimationAsset* GetAnimationAsset() const { return m_Asset; }

		void Play();
		void Stop();

		void SetBool(const std::string& name, bool value);
		void SetInt(const std::string& name, int value);
		void SetFloat(const std::string& name, float value);
		void SetString(const std::string& name, const std::string& value);

		[[nodiscard]] const SpriteFrame* GetCurrentFrame() const;
		[[nodiscard]] bool IsPlaying() const;

		AnimatorParams params;

		const std::vector<PropertyInfo>& GetProperties() const override
		{
			static std::vector<PropertyInfo> properties =
			{
				BUKI_PROP_NESTED_ANIMATION_GROUPED(BUKI_FIELD_NESTED(Animator, "params", params, "animationPath", animationPath), "Asset"),
				BUKI_PROP_NESTED_BOOL_GROUPED(BUKI_FIELD_NESTED(Animator, "params", params, "playOnStart", playOnStart), "Playback"),
				BUKI_PROP_NESTED_FLOAT_GROUPED(BUKI_FIELD_NESTED(Animator, "params", params, "speed", speed), "Playback")
			};

			return properties;
		}

	private:
		AnimationAsset* m_Asset = new AnimationAsset();
		SpriteAnimator m_Player;
		AnimationStateMachine m_StateMachine;
	};
}