#include "Animator.h"

namespace buki
{
    void SpriteAnimator::Play(const AnimationClip* clip)
    {
        if (!clip || clip->frames.empty())
        {
            Stop();
            return;
        }

        m_CurrentClip = clip;
        m_CurrentClipName = clip->name;
        m_FrameIndex = 0;
        m_TimeRemaining = clip->frames.front().duration;
        m_Playing = true;
    }

    void SpriteAnimator::Stop()
    {
        m_Playing = false;
        m_CurrentClip = nullptr;
        m_CurrentClipName.clear();
        m_FrameIndex = 0;
        m_TimeRemaining = 0.0f;
    }

    void SpriteAnimator::Update(float dt)
    {
        if (!m_Playing || !m_CurrentClip || m_CurrentClip->frames.empty())
            return;

        AdvanceFrame(dt);
    }

    void SpriteAnimator::AdvanceFrame(float dt)
    {
        m_TimeRemaining -= dt;

        while (m_TimeRemaining <= 0.0f)
        {
            const SpriteFrame& currentFrame = m_CurrentClip->frames[m_FrameIndex];
            m_TimeRemaining += currentFrame.duration;

            ++m_FrameIndex;

            if (m_FrameIndex >= m_CurrentClip->frames.size())
            {
                if (m_CurrentClip->loop)
                {
                    m_FrameIndex = 0;
                }
                else
                {
                    m_FrameIndex = m_CurrentClip->frames.size() - 1;
                    m_TimeRemaining = 0.0f;
                    m_Playing = false;
                    return;
                }
            }
        }
    }

    const SpriteFrame* SpriteAnimator::GetCurrentFrame() const
    {
        if (!m_CurrentClip || m_CurrentClip->frames.empty())
            return nullptr;

        return &m_CurrentClip->frames[m_FrameIndex];
    }

    void SpriteAnimator::Draw(float alpha)
    {
        const SpriteFrame* frame = GetCurrentFrame();
        if (!frame || !frame->texture)
            return;

        // Replace this with your engine’s sprite/renderer draw call.
        // Example shape:
        // Renderer::DrawTexture(*frame->texture, frame->sourceRect, transform, alpha, frame->flipX, frame->flipY);
    }
}