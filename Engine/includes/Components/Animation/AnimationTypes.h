#pragma once

#include "Graphics/Texture2D.h"
#include "BukiContainers.h"

#include <string>
#include <vector>
#include <variant>

namespace buki
{
	struct SpriteFrame
	{
		std::string texturePath = "";
		Texture2D* texture = nullptr;
		RectF sourceRect{};
		float duration = 0.1f;
		bool flipX = false;
		bool flipY = false;
	};

	struct AnimationClip
	{
		std::string name = "";
		std::vector<SpriteFrame> frames = std::vector<SpriteFrame>();
		bool loop = true;
	};

	// ------- new system for animation state machine --------

	struct AnimatorParams
	{
		bool playOnStart = false;
		float speed = 1.0f;
		std::string animationPath = "";
	};

	enum class AnimParameterType
	{
		Bool,
		Int,
		Float,
		String
	};

	enum class AnimConditionOp
	{
		Equals,
		NotEquals,
		Greater,
		Less,
		GreaterOrEqual,
		LessOrEqual
	};

	using AnimParameterValue = std::variant<bool, int, float, std::string>;

	struct AnimParameter
	{
		std::string name;
		AnimParameterType type = AnimParameterType::Bool;
		AnimParameterValue defaultValue = false;
	};

	struct AnimCondition
	{
		std::string parameter;
		AnimParameterType type = AnimParameterType::Bool;
		AnimConditionOp op = AnimConditionOp::Equals;
		AnimParameterValue value = false;
	};

	struct AnimationTransition
	{
		std::string to;
		std::vector<AnimCondition> conditions;
		bool forceRestart = false;
	};

	struct AnimationState
	{
		std::string name;
		std::string clipName;
		std::vector<AnimationTransition> transitions;
	};

}