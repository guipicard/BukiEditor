#pragma once

#include "Graphics/Texture2D.h"
#include "BukiContainers.h"
#include "Ids.h"

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

	// for animator Serializable fields
	struct AnimatorParams
	{
		std::string animationPath = "";
		bool playOnStart = true;
		float speed = 1.0f;
	};

	// ------- new system for animation state machine --------


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

	struct AnimCondition
	{
		std::string parameter;
		AnimParameterType type = AnimParameterType::Bool;
		AnimConditionOp op = AnimConditionOp::Equals;
		AnimParameterValue value = false;
	};

	struct AnimationTransition
	{
		EditorId id = InvalidEditorId;
		std::string to;
		bool forceRestart = false;
		std::vector<AnimCondition> conditions;
	};

	struct AnimationState
	{
		EditorId id = InvalidEditorId;
		std::string name;
		std::string clipPath;
		std::vector<AnimationTransition> transitions;
	};

	struct AnimParameter
	{
		EditorId id = InvalidEditorId;
		std::string name;
		AnimParameterType type;
		AnimParameterValue defaultValue;
	};

}