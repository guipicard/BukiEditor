#include "AnimationStateMachine.h"
#include "Animator.h"

#include <variant>

namespace buki
{
    void AnimationStateMachine::SetAsset(const AnimationAsset* asset)
    {
        m_Asset = asset;
    }

    void AnimationStateMachine::SetPlayer(SpriteAnimator* player)
    {
        m_Player = player;
    }

    void AnimationStateMachine::SetBool(const std::string& name, bool value)
    {
        m_Parameters[name] = value;
    }

    void AnimationStateMachine::SetInt(const std::string& name, int value)
    {
        m_Parameters[name] = value;
    }

    void AnimationStateMachine::SetFloat(const std::string& name, float value)
    {
        m_Parameters[name] = value;
    }

    void AnimationStateMachine::SetString(const std::string& name, const std::string& value)
    {
        m_Parameters[name] = value;
    }

    void AnimationStateMachine::Start()
    {
        if (!m_Asset)
            return;

        m_Parameters.clear();

        for (const AnimParameter& parameter : m_Asset->parameters)
            m_Parameters[parameter.name] = parameter.defaultValue;

        if (!m_Asset->initialState.empty())
            ChangeState(m_Asset->initialState, true);
    }

    void AnimationStateMachine::Update()
    {
        if (!m_Asset || !m_Player)
            return;

        const AnimationState* currentState = m_Asset->GetState(m_CurrentState);
        if (!currentState)
            return;

        for (const AnimationTransition& transition : currentState->transitions)
        {
            if (EvaluateTransition(transition))
            {
                ChangeState(transition.to, transition.forceRestart);
                return;
            }
        }
    }

    bool AnimationStateMachine::EvaluateTransition(const AnimationTransition& transition) const
    {
        if (transition.to.empty())
            return false;

        if (transition.conditions.empty())
            return true;

        for (const AnimCondition& condition : transition.conditions)
        {
            if (!EvaluateCondition(condition))
                return false;
        }

        return true;
    }

    bool AnimationStateMachine::EvaluateCondition(const AnimCondition& condition) const
    {
        auto it = m_Parameters.find(condition.parameter);
        if (it == m_Parameters.end())
            return false;

        const AnimParameterValue& currentValue = it->second;

        switch (condition.type)
        {
        case AnimParameterType::Bool:
        {
            const bool* current = std::get_if<bool>(&currentValue);
            const bool* target = std::get_if<bool>(&condition.value);
            if (!current || !target)
                return false;

            switch (condition.op)
            {
            case AnimConditionOp::Equals: return *current == *target;
            case AnimConditionOp::NotEquals: return *current != *target;
            default: return false;
            }
        }

        case AnimParameterType::Int:
        {
            const int* current = std::get_if<int>(&currentValue);
            const int* target = std::get_if<int>(&condition.value);
            if (!current || !target)
                return false;

            switch (condition.op)
            {
            case AnimConditionOp::Equals: return *current == *target;
            case AnimConditionOp::NotEquals: return *current != *target;
            case AnimConditionOp::Greater: return *current > *target;
            case AnimConditionOp::Less: return *current < *target;
            case AnimConditionOp::GreaterOrEqual: return *current >= *target;
            case AnimConditionOp::LessOrEqual: return *current <= *target;
            }

            return false;
        }

        case AnimParameterType::Float:
        {
            const float* current = std::get_if<float>(&currentValue);
            const float* target = std::get_if<float>(&condition.value);
            if (!current || !target)
                return false;

            switch (condition.op)
            {
            case AnimConditionOp::Equals: return *current == *target;
            case AnimConditionOp::NotEquals: return *current != *target;
            case AnimConditionOp::Greater: return *current > *target;
            case AnimConditionOp::Less: return *current < *target;
            case AnimConditionOp::GreaterOrEqual: return *current >= *target;
            case AnimConditionOp::LessOrEqual: return *current <= *target;
            }

            return false;
        }

        case AnimParameterType::String:
        {
            const std::string* current = std::get_if<std::string>(&currentValue);
            const std::string* target = std::get_if<std::string>(&condition.value);
            if (!current || !target)
                return false;

            switch (condition.op)
            {
            case AnimConditionOp::Equals: return *current == *target;
            case AnimConditionOp::NotEquals: return *current != *target;
            default: return false;
            }
        }
        }

        return false;
    }

    void AnimationStateMachine::ChangeState(const std::string& stateName, bool forceRestart)
    {
        if (!m_Asset || !m_Player)
            return;

        const AnimationState* nextState = m_Asset->GetState(stateName);
        if (!nextState)
            return;

        if (!forceRestart && m_CurrentState == stateName)
            return;

        m_CurrentState = stateName;

        const AnimationClip* clip = m_Asset->GetClip(nextState->clipName);
        m_Player->Play(clip);
    }
}