#include "AnimationAsset.h"
#include "Engine.h"

#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
namespace fs = std::filesystem;
using json = nlohmann::json;

namespace buki
{
	static void ToJson(const AnimParameterValue& value, json& out)
	{
		if (auto* v = std::get_if<bool>(&value)) out = *v;
		else if (auto* v = std::get_if<int>(&value)) out = *v;
		else if (auto* v = std::get_if<float>(&value)) out = *v;
		else if (auto* v = std::get_if<std::string>(&value)) out = *v;
	}

	static AnimParameterValue ParseValue(const json& value, AnimParameterType type)
	{
		switch (type)
		{
		case AnimParameterType::Bool:
			return value.get<bool>();

		case AnimParameterType::Int:
			return value.get<int>();

		case AnimParameterType::Float:
			return value.get<float>();

		case AnimParameterType::String:
			return value.get<std::string>();
		}

		return false;
	}

	static void ToJson(const SpriteFrame& frame, json& out)
	{
		out["path"] = frame.texturePath;
		out["duration"] = frame.duration;
		out["flipX"] = frame.flipX;
		out["flipY"] = frame.flipY;

		out["sourceRect"]["x"] = frame.sourceRect.x;
		out["sourceRect"]["y"] = frame.sourceRect.y;
		out["sourceRect"]["w"] = frame.sourceRect.w;
		out["sourceRect"]["h"] = frame.sourceRect.h;

		// Texture ownership/loading is engine-specific.
		// If you serialize texture paths elsewhere, store that path here instead.
	}

	static SpriteFrame ParseFrame(const json& frame)
	{
		SpriteFrame result;

		result.texturePath = frame.value("path", "");
		result.duration = frame.value("duration", 0.1f);
		result.flipX = frame.value("flipX", false);
		result.flipY = frame.value("flipY", false);

		if (frame.contains("sourceRect"))
		{
			const json& rect = frame["sourceRect"];
			result.sourceRect.x = rect.value("x", 0.0f);
			result.sourceRect.y = rect.value("y", 0.0f);
			result.sourceRect.w = rect.value("w", 0.0f);
			result.sourceRect.h = rect.value("h", 0.0f);
		}
		return result;
	}

	static void ToJson(const AnimationClip& clip, json& out)
	{
		out["name"] = clip.name;
		out["loop"] = clip.loop;

		json frames = json::array();
		for (const SpriteFrame& frame : clip.frames)
		{
			json frameJson;
			ToJson(frame, frameJson);
			frames.push_back(frameJson);
		}

		out["frames"] = frames;
	}

	static AnimationClip ParseClip(const json& clip)
	{
		AnimationClip result;
		result.name = clip.value("name", "");
		result.loop = clip.value("loop", true);

		if (clip.contains("frames") && clip["frames"].is_array())
		{
			for (const json& frameJson : clip["frames"])
				result.frames.push_back(ParseFrame(frameJson));
		}

		return result;
	}

	static void ToJson(const AnimParameter& parameter, json& out)
	{
		out["name"] = parameter.name;
		out["type"] = static_cast<int>(parameter.type);
		ToJson(parameter.defaultValue, out["defaultValue"]);
	}

	static AnimParameter ParseParameter(const json& parameter)
	{
		AnimParameter result;
		result.name = parameter.value("name", "");
		result.type = static_cast<AnimParameterType>(parameter.value("type", 0));
		result.defaultValue = ParseValue(parameter.value("defaultValue", json(false)), result.type);
		return result;
	}

	static void ToJson(const AnimCondition& condition, json& out)
	{
		out["parameter"] = condition.parameter;
		out["type"] = static_cast<int>(condition.type);
		out["op"] = static_cast<int>(condition.op);
		ToJson(condition.value, out["value"]);
	}

	static AnimCondition ParseCondition(const json& condition)
	{
		AnimCondition result;
		result.parameter = condition.value("parameter", "");
		result.type = static_cast<AnimParameterType>(condition.value("type", 0));
		result.op = static_cast<AnimConditionOp>(condition.value("op", 0));
		result.value = ParseValue(condition.value("value", json(false)), result.type);
		return result;
	}

	static void ToJson(const AnimationTransition& transition, json& out)
	{
		out["to"] = transition.to;
		out["forceRestart"] = transition.forceRestart;

		json conditions = json::array();
		for (const AnimCondition& condition : transition.conditions)
		{
			json conditionJson;
			ToJson(condition, conditionJson);
			conditions.push_back(conditionJson);
		}

		out["conditions"] = conditions;
	}

	static AnimationTransition ParseTransition(const json& transition)
	{
		AnimationTransition result;
		result.to = transition.value("to", "");
		result.forceRestart = transition.value("forceRestart", false);

		if (transition.contains("conditions") && transition["conditions"].is_array())
		{
			for (const json& conditionJson : transition["conditions"])
				result.conditions.push_back(ParseCondition(conditionJson));
		}

		return result;
	}

	static void ToJson(const AnimationState& state, json& out)
	{
		out["name"] = state.name;
		out["clipName"] = state.clipName;

		json transitions = json::array();
		for (const AnimationTransition& transition : state.transitions)
		{
			json transitionJson;
			ToJson(transition, transitionJson);
			transitions.push_back(transitionJson);
		}

		out["transitions"] = transitions;
	}

	static AnimationState ParseState(const json& state)
	{
		AnimationState result;
		result.name = state.value("name", "");
		result.clipName = state.value("clipName", "");

		if (state.contains("transitions") && state["transitions"].is_array())
		{
			for (const json& transitionJson : state["transitions"])
				result.transitions.push_back(ParseTransition(transitionJson));
		}

		return result;
	}

	bool AnimationAsset::SaveToFile(const std::string& path) const
	{
		json root;
		root["version"] = version;
		root["name"] = name;
		root["initialState"] = initialState;

		json parametersJson = json::array();
		for (const AnimParameter& parameter : parameters)
		{
			json parameterJson;
			ToJson(parameter, parameterJson);
			parametersJson.push_back(parameterJson);
		}
		root["parameters"] = parametersJson;

		json statesJson = json::array();
		for (const AnimationState& state : states)
		{
			json stateJson;
			ToJson(state, stateJson);
			statesJson.push_back(stateJson);
		}
		root["states"] = statesJson;

		json clipsJson = json::object();
		for (const auto& clip : clips)
		{
			json clipJson;
			ToJson(clip, clipJson);
			clipsJson[clip.name] = clipJson;
		}
		root["clips"] = clipsJson;

		std::ofstream file(path);
		if (!file.is_open())
			return false;

		file << root.dump(4);
		return true;
	}

	bool AnimationAsset::LoadFromFile(const std::string& path)
	{
		const fs::path absolutePath =
			fs::absolute(fs::path("../Deployment") / path).lexically_normal();

		std::ifstream file(absolutePath);
		if (!file.is_open())
		{
			return false;
		}

		json root;
		try
		{
			file >> root;
		}
		catch (const json::exception& e)
		{
			return false;
		}

		file.close();

		if (!root.is_object())
		{
			return false;
		}

		version = root.value("version", 1);
		name = root.value("name", "");
		initialState = root.value("initialState", "");

		parameters.clear();
		states.clear();
		clips.clear();

		if (root.contains("parameters") && root["parameters"].is_array())
		{
			for (const json& parameterJson : root["parameters"])
				parameters.push_back(ParseParameter(parameterJson));
		}

		if (root.contains("states") && root["states"].is_array())
		{
			for (const json& stateJson : root["states"])
				states.push_back(ParseState(stateJson));
		}

		if (root.contains("clips") && root["clips"].is_object())
		{
			for (auto it = root["clips"].begin(); it != root["clips"].end(); ++it)
				clips.push_back(ParseClip(it.value()));
		}

		return true;
	}

	const AnimationState* AnimationAsset::GetState(const std::string& stateName) const
	{
		for (const AnimationState& state : states)
		{
			if (state.name == stateName)
				return &state;
		}

		return nullptr;
	}

	AnimationClip* AnimationAsset::GetClip(const std::string& clipName) const
	{
		for (AnimationClip clip : clips)
		{
			if (clip.name == clipName)
			{
				return &clip;
			}
		}
		return nullptr;
	}

	const AnimParameter* AnimationAsset::GetParameter(const std::string& parameterName) const
	{
		for (const AnimParameter& parameter : parameters)
		{
			if (parameter.name == parameterName)
				return &parameter;
		}

		return nullptr;
	}
}