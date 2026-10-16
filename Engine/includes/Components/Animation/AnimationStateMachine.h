#pragma once

#include "AnimationTypes.h"
#include "AnimationAsset.h"

#include <string>
#include <unordered_map>

namespace buki
{
	class SpriteAnimator;

	class AnimationStateMachine
	{
	public:
		AnimationStateMachine() = default;

		void SetAsset(const AnimationAsset* asset);
		void SetPlayer(SpriteAnimator* player);

		void SetBool(const std::string& name, bool value);
		void SetInt(const std::string& name, int value);
		void SetFloat(const std::string& name, float value);
		void SetString(const std::string& name, const std::string& value);

		void Start();
		void Update();

		[[nodiscard]] const std::string& GetCurrentState() const { return m_CurrentState; }

	private:
		bool EvaluateCondition(const AnimCondition& condition) const;
		bool EvaluateTransition(const AnimationTransition& transition) const;
		void ChangeState(const std::string& name, bool forceRestart = false);

		const AnimationAsset* m_Asset = nullptr;
		SpriteAnimator* m_Player = nullptr;
		std::unordered_map<std::string, AnimParameterValue> m_Parameters;
		std::string m_CurrentState;
	};
}