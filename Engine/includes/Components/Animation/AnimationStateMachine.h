#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace buki
{
    class SpriteAnimator;

    struct AnimationTransition
    {
        std::string to;
        std::function<bool()> condition;
        bool forceRestart = false;
    };

    struct AnimationState
    {
        std::string name;
        std::string clipName;
        std::vector<AnimationTransition> transitions;
        std::function<void()> onEnter;
        std::function<void()> onExit;
    };

    class AnimationStateMachine
    {
    public:
        explicit AnimationStateMachine(SpriteAnimator* animator);
        ~AnimationStateMachine() = default;

        void AddState(const AnimationState& state);
        void SetInitialState(const std::string& name);
        void Update();

        void ChangeState(const std::string& name, bool forceRestart = false);

        [[nodiscard]] const std::string& GetCurrentState() const { return m_CurrentState; }

    private:
        SpriteAnimator* m_Animator = nullptr;
        std::map<std::string, AnimationState> m_States = std::map<std::string, AnimationState>();
        std::string m_CurrentState;
    };
}
