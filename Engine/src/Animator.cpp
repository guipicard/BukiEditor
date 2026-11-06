#include "Animator.h"
#include "Entity.h"
#include "Engine.h"

namespace buki
{
	Animator::Animator(Entity* entity)
		: Component(entity)
	{
	}

	void Animator::Start()
	{
		m_StateMachine.SetAsset(&m_Asset);
		m_StateMachine.SetPlayer(&m_Player);

		if (!params.animationPath.empty())
		{
			m_Asset.LoadFromFile(params.animationPath);
		}

		if (params.playOnStart)
		{
			m_StateMachine.Start();
		}
	}

	void Animator::Destroy()
	{
		m_Player.Stop();
	}

	void Animator::SetAnimationAsset(AnimationAsset* asset)
	{
		if (asset == nullptr)
			return;

		m_Asset = *asset;
		m_LoadedAnimationPath = params.animationPath;

		m_StateMachine.SetAsset(&m_Asset);
		m_StateMachine.SetPlayer(&m_Player);

		m_StateMachine.Start();
	}

	void Animator::Play()
	{
		m_StateMachine.Start();
	}

	void Animator::Stop()
	{
		m_Player.Stop();
	}

	void Animator::SetFlipX(bool flipX)
	{
		m_Player.SetFlipX(flipX);
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
		m_StateMachine.Update();
		m_Player.Update(dt * params.speed);
	}

	void Animator::Draw(float alpha)
	{
		Vector2 pos = m_Entity->Tm().GetPosition();
		Vector2 size = m_Entity->Tm().GetSize();
		RectF destRect = RectF{ pos.x,pos.y, size.x,size.y };
		m_Player.Draw(alpha, destRect);
	}

	const SpriteFrame* Animator::GetCurrentFrame()
	{
		return m_Player.GetCurrentFrame();
	}

	bool Animator::IsPlaying() const
	{
		return m_Player.IsPlaying();
	}

	void Animator::Set()
	{
		if (params.animationPath.empty())
			return;

		if (m_LoadedAnimationPath == params.animationPath)
			return;

		if (m_Asset.LoadFromFile(params.animationPath))
		{
			m_LoadedAnimationPath = params.animationPath;

			m_StateMachine.SetAsset(&m_Asset);
			m_StateMachine.SetPlayer(&m_Player);
			m_StateMachine.Start();
		}
	}
}