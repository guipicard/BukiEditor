#pragma once

#include "AnimationTypes.h"

#include <map>
#include <string>
#include <vector>

namespace buki
{
	struct AnimationStateLayout
	{
		std::string stateName;
		Vector2 editorPosition{};
	};

	struct AnimationAsset
	{
		int version = 1;
		std::string name;
		std::vector<AnimParameter> parameters;
		std::vector<AnimationState> states;
		std::vector<AnimationClip> clips;
		std::string initialState;

		bool LoadFromFile(const std::string& path);
		bool SaveToFile(const std::string& path) const;

		[[nodiscard]] const AnimationState* GetState(const std::string& name) const;
		[[nodiscard]] AnimationClip* GetClip(const std::string& name) const;
		[[nodiscard]] const AnimParameter* GetParameter(const std::string& name) const;

		// editor-only data
		std::vector<AnimationStateLayout> layout;
	};
}