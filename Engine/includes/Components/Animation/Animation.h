#pragma once

#include "Graphics/Texture2D.h"
#include "BukiContainers.h"

#include "Component.h"
#include "IDrawable.h"
#include "IUpdatable.h"
#include "AnimationStateMachine.h"
#include "AnimationClipLibrary.h"
#include "nlohmann/json_fwd.hpp"
#include "AnimationTypes.h"
#include "AnimParams.h"

#include <string>
#include <vector>
#include <map>
#include <chrono>

using json = nlohmann::json;

namespace buki
{
    class Entity;

    // Animation state machine
    class SpriteAnimator
    {
    public:
        SpriteAnimator();
        ~SpriteAnimator();

        void Play(const std::string& clipName);
        void Play(const AnimationClip& clip);
        void Stop();

		void Draw(float alpha);
        void Update(float dt);

        [[nodiscard]] const SpriteFrame* GetCurrentFrame() const;
        [[nodiscard]] bool IsPlaying() const { return m_Playing; }
        [[nodiscard]] std::string GetCurrentClipName() const { return m_CurrentClipName; }

        json Serialize() const;
        void Deserialize(const json& doc);

        void AddClip(const AnimationClip& clip)
        {
            m_Clips[clip.name] = clip;
		}

    private:
        void AdvanceFrame(float dt);

        std::map<std::string, AnimationClip> m_Clips;
        const AnimationClip* m_CurrentClip = nullptr;
        std::size_t m_FrameIndex = 0;
        float m_TimeRemaining = 0.0f;
        bool m_Playing = false;
        std::string m_CurrentClipName;
    };

    // Animation component that owns a single animator
    struct Animation : public Component, public IDrawable, public IUpdatable
    {
        Animation(Entity* _entity);
        SpriteAnimator m_Animator = SpriteAnimator();
        AnimationStateMachine m_StateMachine;
        AnimationClipLibrary m_ClipLibrary = AnimationClipLibrary();
        virtual void Start() override;
        virtual void Destroy() override;

        virtual void Draw(float alpha) override;

        virtual void Update(float dt) override;

        AnimationParams params;

        void Play(const std::string& clipName)
        {
            m_Animator.Play(clipName);
        }

        void Play(const AnimationClip& clip)
        {
            m_Animator.Play(clip);
        }

        void Stop()
        {
            m_Animator.Stop();
        }

        [[nodiscard]] const SpriteFrame* GetCurrentFrame() const
        {
            return m_Animator.GetCurrentFrame();
        }

        [[nodiscard]] bool IsPlaying() const
        {
            return m_Animator.IsPlaying();
        }

        void AddClip(const AnimationClip& clip)
        {
            m_ClipLibrary.AddClip(clip);
            m_Animator.AddClip(clip);
        }

        virtual json Serialize() override;
        virtual void Deserialize(json doc) override;
        virtual void Set() override;
    };
}
