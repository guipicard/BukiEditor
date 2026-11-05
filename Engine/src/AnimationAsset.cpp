#include "AnimationAsset.h"

#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace buki
{
    static void ToJson(const AnimParameterValue& value, json& out)
    {
        if (auto* v = std::get_if<bool>(&value))
            out = *v;
        else if (auto* v = std::get_if<int>(&value))
            out = *v;
        else if (auto* v = std::get_if<float>(&value))
            out = *v;
        else if (auto* v = std::get_if<std::string>(&value))
            out = *v;
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
        result.type = static_cast<AnimParameterType>(
            parameter.value("type", static_cast<int>(AnimParameterType::Bool)));
        result.defaultValue = ParseValue(
            parameter.value("defaultValue", json(false)),
            result.type);

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
        result.type = static_cast<AnimParameterType>(
            condition.value("type", static_cast<int>(AnimParameterType::Bool)));
        result.op = static_cast<AnimConditionOp>(
            condition.value("op", static_cast<int>(AnimConditionOp::Equals)));
        result.value = ParseValue(
            condition.value("value", json(false)),
            result.type);

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

        if (transition.contains("conditions") &&
            transition["conditions"].is_array())
        {
            for (const json& conditionJson : transition["conditions"])
                result.conditions.push_back(ParseCondition(conditionJson));
        }

        return result;
    }

    static void ToJson(const AnimationState& state, json& out)
    {
        out["name"] = state.name;
        out["clipPath"] = state.clipPath;

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
        result.clipPath = state.value("clipPath", "");

        if (state.contains("transitions") &&
            state["transitions"].is_array())
        {
            for (const json& transitionJson : state["transitions"])
                result.transitions.push_back(ParseTransition(transitionJson));
        }

        return result;
    }

    static void ToJson(const AnimationClipReference& clip, json& out)
    {
        out["clipPath"] = clip.clipPath;
    }

    static AnimationClipReference ParseClipReference(const json& clip)
    {
        AnimationClipReference result;
        result.clipPath = clip.value("clipPath", "");
        return result;
    }

    static void ToJson(const AnimationStateLayout& layout, json& out)
    {
        out["stateName"] = layout.stateName;
        out["position"]["x"] = layout.editorPosition.x;
        out["position"]["y"] = layout.editorPosition.y;
    }

    static AnimationStateLayout ParseLayout(const json& layout)
    {
        AnimationStateLayout result;

        result.stateName = layout.value("stateName", "");

        if (layout.contains("position"))
        {
            result.editorPosition.x = layout["position"].value("x", 0.0f);
            result.editorPosition.y = layout["position"].value("y", 0.0f);
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

        json clipsJson = json::array();

        for (const AnimationClipReference& clip : clips)
        {
            json clipJson;
            ToJson(clip, clipJson);
            clipsJson.push_back(clipJson);
        }

        root["clips"] = clipsJson;

        json layoutJson = json::array();

        for (const AnimationStateLayout& layout : layout)
        {
            json layoutJsonItem;
            ToJson(layout, layoutJsonItem);
            layoutJson.push_back(layoutJsonItem);
        }

        root["layout"] = layoutJson;

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
            return false;

        json root;

        try
        {
            file >> root;
        }
        catch (const json::exception&)
        {
            return false;
        }

        if (!root.is_object())
            return false;

        version = root.value("version", 1);
        name = root.value("name", "");
        initialState = root.value("initialState", "");

        parameters.clear();
        states.clear();
        clips.clear();
        layout.clear();

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

        if (root.contains("clips") && root["clips"].is_array())
        {
            for (const json& clipJson : root["clips"])
                clips.push_back(ParseClipReference(clipJson));
        }

        if (root.contains("layout") && root["layout"].is_array())
        {
            for (const json& layoutJson : root["layout"])
                layout.push_back(ParseLayout(layoutJson));
        }

        return true;
    }

    const AnimationState* AnimationAsset::GetState(
        const std::string& stateName) const
    {
        for (const AnimationState& state : states)
        {
            if (state.name == stateName)
                return &state;
        }

        return nullptr;
    }

    const AnimationClipReference* AnimationAsset::GetClipReference(
        const std::string& clipPath) const
    {
        for (const AnimationClipReference& clip : clips)
        {
            if (clip.clipPath == clipPath)
                return &clip;
        }

        return nullptr;
    }

    const AnimParameter* AnimationAsset::GetParameter(
        const std::string& parameterName) const
    {
        for (const AnimParameter& parameter : parameters)
        {
            if (parameter.name == parameterName)
                return &parameter;
        }

        return nullptr;
    }
}