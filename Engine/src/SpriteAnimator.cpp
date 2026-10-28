#include "Animator.h"
#include "Engine.h"
#include "BukiContainers.h"

namespace buki
{
    void SpriteAnimator::Play(AnimationClip* clip)
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

    void SpriteAnimator::Draw(float alpha, const RectF& destRect)
    {
        const SpriteFrame* frame = GetCurrentFrame();
        if (!frame || !frame->texture)
            return;

        bool flipX = frame->flipX != m_FlipX;
        bool flipY = frame->flipY;
        const RectF &srcRect = frame->sourceRect;
        RectF scaledRect = destRect;
        scaledRect.w *= srcRect.w / METRES_TO_PIXELS;
        scaledRect.h *= srcRect.h / METRES_TO_PIXELS;
        float rad = 0.0f;
        Engine::Get().Graphics().DrawSprite(*frame->texture, scaledRect, srcRect, rad, flipX, flipY, Color{1.0f,1.0f,1.0f,1.0f});
    }

    void SpriteAnimator::SetFlipX(bool flipX)
    {
        m_FlipX = flipX;
    }
}