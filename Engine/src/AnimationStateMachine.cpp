#include "AnimationStateMachine.h"
#include "Animation.h"

namespace buki
{
    AnimationStateMachine::AnimationStateMachine(SpriteAnimator* animator)
        : m_Animator(animator)
    {
    }

    void AnimationStateMachine::AddState(const AnimationState& state)
    {
        m_States[state.name] = state;
    }

    void AnimationStateMachine::SetInitialState(const std::string& name)
    {
        auto it = m_States.find(name);
        if (it == m_States.end() || m_Animator == nullptr)
        {
            return;
        }

        m_CurrentState = name;

        if (it->second.onEnter)
        {
            it->second.onEnter();
        }

        m_Animator->Play(it->second.clipName);
    }

    void AnimationStateMachine::Update()
    {
        if (m_CurrentState.empty())
        {
            return;
        }

        auto stateIt = m_States.find(m_CurrentState);
        if (stateIt == m_States.end())
        {
            return;
        }

        AnimationState& state = stateIt->second;

        for (const AnimationTransition& transition : state.transitions)
        {
            if (transition.condition && transition.condition())
            {
                ChangeState(transition.to, transition.forceRestart);
                return;
            }
        }
    }

    void AnimationStateMachine::ChangeState(const std::string& name, bool forceRestart)
    {
        auto nextIt = m_States.find(name);
        if (nextIt == m_States.end() || m_Animator == nullptr)
        {
            return;
        }

        if (!forceRestart && m_CurrentState == name)
        {
            return;
        }

        if (!m_CurrentState.empty())
        {
            auto currentIt = m_States.find(m_CurrentState);
            if (currentIt != m_States.end() && currentIt->second.onExit)
            {
                currentIt->second.onExit();
            }
        }

        m_CurrentState = name;

        if (nextIt->second.onEnter)
        {
            nextIt->second.onEnter();
        }

        m_Animator->Play(nextIt->second.clipName);
    }
}
