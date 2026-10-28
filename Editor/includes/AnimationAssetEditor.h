#pragma once

#include "AnimationAsset.h"
#include <string>

namespace buki
{
	struct AnimationAssetEditor
	{
		std::string assetPath;
		AnimationAsset asset;
		bool open = false;

		bool Load(const std::string& path);
		bool Save();
		void DrawInspector();

		static void DrawConditionOperatorEditor(AnimCondition& condition);
		static void DrawConditionValueEditor(AnimCondition& condition);

	private:
		void DrawAssetSection();
		void DrawParametersSection();
		void DrawStatesSection();
		void DrawTransitionsForState(AnimationState& state);
		void DrawClipsSection();

	};
}