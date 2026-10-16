#include "Animator.h"

namespace buki
{
    Animator::Animator(Entity* entity)
        : Component(entity)
    {}

    void Animator::Start()
    {
        m_StateMachine.SetAsset(m_Asset);
        m_StateMachine.SetPlayer(&m_Player);

        if (m_Asset && params.playOnStart)
        {
            m_StateMachine.Start();
        }
    }

    void Animator::Destroy()
    {
        m_StateMachine.SetAsset(nullptr);
        m_StateMachine.SetPlayer(nullptr);
        m_Player.Stop();
    }

    void Animator::SetAnimationAsset(AnimationAsset* asset)
    {
        m_Asset = asset;

        m_StateMachine.SetAsset(m_Asset);
        m_StateMachine.SetPlayer(&m_Player);

        if (m_Asset)
            m_StateMachine.Start();
    }

    void Animator::Play()
    {
        if (m_Asset)
            m_StateMachine.Start();
    }

    void Animator::Stop()
    {
        m_Player.Stop();
    }

    void Animator::SetBool(const std::string& name, bool value)
    {
        m_StateMachine.SetBool(name, value);
    }

    void Animator::SetInt(const std::string& name, int value)
    {
        m_StateMachine.SetInt(name, value);
    }

    void Animator::SetFloat(const std::string& name, float value)
    {
        m_StateMachine.SetFloat(name, value);
    }

    void Animator::SetString(const std::string& name, const std::string& value)
    {
        m_StateMachine.SetString(name, value);
    }

    void Animator::Update(float dt)
    {
        if (!m_Asset)
            return;

        m_StateMachine.Update();
        m_Player.Update(dt * params.speed);
    }

    void Animator::Draw(float alpha)
    {
        m_Player.Draw(alpha);
    }

    const SpriteFrame* Animator::GetCurrentFrame() const
    {
        return m_Player.GetCurrentFrame();
    }

    bool Animator::IsPlaying() const
    {
        return m_Player.IsPlaying();
    }

    void Animator::Set()
    {
        // Add editor-only behavior here if needed.
        // For example: reload the asset when animationAssetPath changes.
    }
}