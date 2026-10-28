#pragma once
#include "AnimationassetEditor.h"
#include "InspectorPropertyDrawer.h"
#include "imgui.h"
#include "PropertyInfo.h"

bool buki::AnimationAssetEditor::Load(const std::string& path)
{
	buki::AnimationAsset loaded;
	if (!loaded.LoadFromFile(path))
		return false;

	assetPath = path;
	asset = std::move(loaded);
	open = true;
	return true;
}

bool buki::AnimationAssetEditor::Save()
{
	return asset.SaveToFile(assetPath);
}

static const char* GetConditionOpName(buki::AnimConditionOp op)
{
	using buki::AnimConditionOp;

	switch (op)
	{
	case AnimConditionOp::Equals: return "==";
	case AnimConditionOp::NotEquals: return "!=";
	case AnimConditionOp::Greater: return ">";
	case AnimConditionOp::Less: return "<";
	case AnimConditionOp::GreaterOrEqual: return ">=";
	case AnimConditionOp::LessOrEqual: return "<=";
	}

	return "==";
}

static void DrawParameterValueEditor(
	buki::AnimParameterValue& value,
	buki::AnimParameterType type)
{
	using buki::AnimParameterType;

	switch (type)
	{
	case AnimParameterType::Bool:
	{
		bool typedValue = std::get<bool>(value);

		if (ImGui::Checkbox("Default", &typedValue))
			value = typedValue;

		break;
	}

	case AnimParameterType::Int:
	{
		int typedValue = std::get<int>(value);

		if (ImGui::InputInt("Default", &typedValue))
			value = typedValue;

		break;
	}

	case AnimParameterType::Float:
	{
		float typedValue = std::get<float>(value);

		if (ImGui::InputFloat("Default", &typedValue))
			value = typedValue;

		break;
	}

	case AnimParameterType::String:
	{
		char buffer[256];
		std::snprintf(buffer, sizeof(buffer), "%s",
			std::get<std::string>(value).c_str());

		if (ImGui::InputText("Default", buffer, sizeof(buffer)))
			value = std::string(buffer);

		break;
	}
	}
}



void buki::AnimationAssetEditor::DrawInspector()
{
	if (!open)
		return;

	DrawAssetSection();
	ImGui::Separator();

	DrawParametersSection();
	ImGui::Separator();

	DrawStatesSection();
	ImGui::Separator();

	DrawClipsSection();
	ImGui::Separator();

	if (ImGui::Button("Save"))
		Save();
}

void buki::AnimationAssetEditor::DrawAssetSection()
{
	ImGui::SeparatorText("Animation Asset");

	char nameBuffer[256];
	std::snprintf(nameBuffer, sizeof(nameBuffer), "%s", asset.name.c_str());

	if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
		asset.name = nameBuffer;

	// Initial state should be chosen from existing states.
	if (ImGui::BeginCombo("Initial State", asset.initialState.c_str()))
	{
		for (const AnimationState& state : asset.states)
		{
			const bool isSelected = (state.name == asset.initialState);

			if (ImGui::Selectable(state.name.c_str(), isSelected))
				asset.initialState = state.name;

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

void buki::AnimationAssetEditor::DrawParametersSection()
{
	ImGui::SeparatorText("Parameters");

	if (asset.parameters.empty())
	{
		ImGui::TextDisabled("No parameters. Add one to use it in transitions.");
	}

	if (ImGui::Button("Add Parameter"))
	{
		AnimParameter parameter;
		parameter.name = "NewParameter";
		parameter.type = AnimParameterType::Float;
		parameter.defaultValue = 0.0f;

		asset.parameters.push_back(parameter);
	}

	int parameterToRemove = -1;

	for (int parameterIndex = 0;
		parameterIndex < static_cast<int>(asset.parameters.size());
		++parameterIndex)
	{
		AnimParameter& parameter = asset.parameters[parameterIndex];

		ImGui::PushID(parameterIndex);

		if (ImGui::TreeNode("Parameter"))
		{
			char nameBuffer[256];
			std::snprintf(nameBuffer, sizeof(nameBuffer), "%s", parameter.name.c_str());

			if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
				parameter.name = nameBuffer;

			int typeIndex = static_cast<int>(parameter.type);

			const char* typeNames[] =
			{
				"Bool",
				"Int",
				"Float",
				"String"
			};

			if (ImGui::Combo("Type", &typeIndex, typeNames, IM_ARRAYSIZE(typeNames)))
			{
				parameter.type = static_cast<AnimParameterType>(typeIndex);

				switch (parameter.type)
				{
				case AnimParameterType::Bool:
					parameter.defaultValue = false;
					break;

				case AnimParameterType::Int:
					parameter.defaultValue = 0;
					break;

				case AnimParameterType::Float:
					parameter.defaultValue = 0.0f;
					break;

				case AnimParameterType::String:
					parameter.defaultValue = std::string();
					break;
				}
			}

			DrawParameterValueEditor(parameter.defaultValue, parameter.type);

			if (ImGui::Button("Remove Parameter"))
				parameterToRemove = parameterIndex;

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	if (parameterToRemove >= 0)
		asset.parameters.erase(asset.parameters.begin() + parameterToRemove);
}



void buki::AnimationAssetEditor::DrawStatesSection()
{
	ImGui::SeparatorText("States & Transitions");

	if (asset.states.empty())
	{
		ImGui::TextDisabled("No states. Add one to begin building the graph.");
	}

	if (ImGui::Button("Add State"))
	{
		AnimationState state;
		state.name = "NewState";
		state.clipName = "";

		asset.states.push_back(state);
	}

	int stateToRemove = -1;

	for (int stateIndex = 0;
		stateIndex < static_cast<int>(asset.states.size());
		++stateIndex)
	{
		AnimationState& state = asset.states[stateIndex];

		ImGui::PushID(stateIndex);

		const std::string treeNodeId =
			"State##" + std::to_string(stateIndex);

		if (ImGui::TreeNode(treeNodeId.c_str()))
		{
			char nameBuffer[256];
			std::snprintf(nameBuffer, sizeof(nameBuffer), "%s", state.name.c_str());

			if (ImGui::InputText("State Name", nameBuffer, sizeof(nameBuffer)))
				state.name = nameBuffer;

			// Clip selection should come from existing clips.
			if (ImGui::BeginCombo("Clip", state.clipName.c_str()))
			{
				for (const AnimationClip& clip : asset.clips)
				{
					const bool isSelected = (clip.name == state.clipName);

					if (ImGui::Selectable(clip.name.c_str(), isSelected))
						state.clipName = clip.name;

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::Separator();
			ImGui::TextDisabled("Transitions");

			DrawTransitionsForState(state);

			ImGui::Separator();

			if (ImGui::Button("Remove State"))
				stateToRemove = stateIndex;

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	if (stateToRemove >= 0)
		asset.states.erase(asset.states.begin() + stateToRemove);
}

void buki::AnimationAssetEditor::DrawTransitionsForState(
	AnimationState& state)
{
	if (ImGui::Button("Add Transition"))
	{
		AnimationTransition transition;

		for (const AnimationState& otherState : asset.states)
		{
			if (otherState.name != state.name)
			{
				transition.to = otherState.name;
				break;
			}
		}

		state.transitions.push_back(transition);
	}

	int transitionToRemove = -1;

	for (int transitionIndex = 0;
		transitionIndex < static_cast<int>(state.transitions.size());
		++transitionIndex)
	{
		AnimationTransition& transition = state.transitions[transitionIndex];

		ImGui::PushID(transitionIndex);

		if (ImGui::TreeNode("Transition"))
		{
			if (ImGui::BeginCombo("To State", transition.to.c_str()))
			{
				for (const AnimationState& otherState : asset.states)
				{
					const bool isSelected =
						(otherState.name == transition.to);

					if (ImGui::Selectable(otherState.name.c_str(), isSelected))
						transition.to = otherState.name;

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::Checkbox("Force Restart", &transition.forceRestart);

			ImGui::Separator();
			ImGui::TextDisabled("Conditions");

			if (ImGui::Button("Add Condition"))
			{
				AnimCondition condition;

				if (!asset.parameters.empty())
				{
					const AnimParameter& firstParameter =
						asset.parameters.front();

					condition.parameter = firstParameter.name;
					condition.type = firstParameter.type;
					condition.value = firstParameter.defaultValue;
				}

				transition.conditions.push_back(condition);
			}

			int conditionToRemove = -1;

			for (int conditionIndex = 0;
				conditionIndex < static_cast<int>(transition.conditions.size());
				++conditionIndex)
			{
				AnimCondition& condition =
					transition.conditions[conditionIndex];

				ImGui::PushID(conditionIndex);

				if (ImGui::BeginCombo("Parameter", condition.parameter.c_str()))
				{
					for (const AnimParameter& parameter : asset.parameters)
					{
						const bool isSelected =
							(parameter.name == condition.parameter);

						if (ImGui::Selectable(parameter.name.c_str(), isSelected))
						{
							condition.parameter = parameter.name;
							condition.type = parameter.type;
							condition.value = parameter.defaultValue;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				DrawConditionOperatorEditor(condition);
				DrawConditionValueEditor(condition);

				if (ImGui::Button("Remove Condition"))
					conditionToRemove = conditionIndex;

				ImGui::PopID();
			}

			if (conditionToRemove >= 0)
			{
				transition.conditions.erase(
					transition.conditions.begin() + conditionToRemove);
			}

			if (ImGui::Button("Remove Transition"))
				transitionToRemove = transitionIndex;

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	if (transitionToRemove >= 0)
	{
		state.transitions.erase(
			state.transitions.begin() + transitionToRemove);
	}
}

void buki::AnimationAssetEditor::DrawConditionValueEditor(AnimCondition& condition)
{
	using buki::AnimConditionOp;
	using buki::AnimParameterType;

	switch (condition.type)
	{
	case AnimParameterType::Bool:
	{
		bool value = std::get<bool>(condition.value);
		if (ImGui::Checkbox("Value", &value))
			condition.value = value;
		break;
	}

	case AnimParameterType::Int:
	{
		int value = std::get<int>(condition.value);
		if (ImGui::InputInt("Value", &value))
			condition.value = value;
		break;
	}

	case AnimParameterType::Float:
	{
		float value = std::get<float>(condition.value);
		if (ImGui::InputFloat("Value", &value))
			condition.value = value;
		break;
	}

	case AnimParameterType::String:
	{
		char buffer[256];
		std::snprintf(buffer, sizeof(buffer), "%s",
			std::get<std::string>(condition.value).c_str());

		if (ImGui::InputText("Value", buffer, sizeof(buffer)))
			condition.value = std::string(buffer);
		break;
	}
	}
}

void buki::AnimationAssetEditor::DrawConditionOperatorEditor(
	AnimCondition& condition)
{
	const char* currentOpName = GetConditionOpName(condition.op);

	if (ImGui::BeginCombo("Operation", currentOpName))
	{
		for (int opIndex = 0;
			opIndex <= static_cast<int>(AnimConditionOp::LessOrEqual);
			++opIndex)
		{
			const AnimConditionOp op =
				static_cast<AnimConditionOp>(opIndex);

			const bool isSelected = (condition.op == op);

			if (ImGui::Selectable(GetConditionOpName(op), isSelected))
				condition.op = op;

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

void buki::AnimationAssetEditor::DrawClipsSection()
{
	ImGui::SeparatorText("Clips");

	if (asset.clips.empty())
	{
		ImGui::TextDisabled("No clips. Add one before assigning it to a state.");
	}

	if (ImGui::Button("Add Clip"))
	{
		AnimationClip clip;
		clip.name = "NewClip";
		clip.loop = true;

		asset.clips.push_back(clip);
	}

	int clipToRemove = -1;

	for (int clipIndex = 0;
		clipIndex < static_cast<int>(asset.clips.size());
		++clipIndex)
	{
		AnimationClip& clip = asset.clips[clipIndex];

		ImGui::PushID(clipIndex);

		const std::string treeNodeId =
			"Clip##" + std::to_string(clipIndex);

		if (ImGui::TreeNode(treeNodeId.c_str()))
		{
			char nameBuffer[256];
			std::snprintf(nameBuffer, sizeof(nameBuffer), "%s", clip.name.c_str());

			if (ImGui::InputText("Clip Name", nameBuffer, sizeof(nameBuffer)))
				clip.name = nameBuffer;

			ImGui::Checkbox("Loop", &clip.loop);

			ImGui::Separator();
			ImGui::TextDisabled("Frames");

			if (ImGui::Button("Add Frame"))
			{
				SpriteFrame frame;
				frame.texturePath = "";
				frame.duration = 0.1f;

				clip.frames.push_back(frame);
			}

			int frameToRemove = -1;

			for (int frameIndex = 0;
				frameIndex < static_cast<int>(clip.frames.size());
				++frameIndex)
			{
				SpriteFrame& frame = clip.frames[frameIndex];

				ImGui::PushID(frameIndex);

				if (ImGui::TreeNode("Frame"))
				{
					PropertyInfo prop =
						BUKI_PROP_STRING(SpriteFrame, texturePath);

					InspectorPropertyDrawer::DrawStringAssetPicker(
						prop,
						&frame.texturePath,
						"IMAGE"
					);

					ImGui::DragFloat(
						"Duration",
						&frame.duration,
						0.01f,
						0.01f,
						10.0f
					);

					ImGui::Checkbox("Flip X", &frame.flipX);
					ImGui::Checkbox("Flip Y", &frame.flipY);

					if (ImGui::Button("Remove Frame"))
						frameToRemove = frameIndex;

					ImGui::TreePop();
				}

				ImGui::PopID();
			}

			if (frameToRemove >= 0)
			{
				clip.frames.erase(
					clip.frames.begin() + frameToRemove);
			}

			if (ImGui::Button("Remove Clip"))
				clipToRemove = clipIndex;

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	if (clipToRemove >= 0)
		asset.clips.erase(asset.clips.begin() + clipToRemove);
}