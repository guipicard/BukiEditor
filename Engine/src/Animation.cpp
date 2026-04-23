#include "Animation.h"

#include "Engine.h"
#include "Entity.h"
#include "Units.h"
#include "nlohmann/json.hpp"

namespace buki
{
	SpriteAnimator::SpriteAnimator()
	{
	}

	SpriteAnimator::~SpriteAnimator()
	{
	}

	void SpriteAnimator::Play(const std::string& clipName)
	{
		auto it = m_Clips.find(clipName);
		if (it == m_Clips.end())
		{
			return;
		}

		m_CurrentClip = &it->second;
		m_FrameIndex = 0;
		m_TimeRemaining = m_CurrentClip->frames.front().duration;
		m_Playing = true;
		m_CurrentClipName = clipName;
	}

	void SpriteAnimator::Play(const AnimationClip& clip)
	{
		m_CurrentClip = &clip;
		m_FrameIndex = 0;
		m_TimeRemaining = m_CurrentClip->frames.front().duration;
		m_Playing = true;
		m_CurrentClipName = clip.name;
	}

	void SpriteAnimator::Stop()
	{
		m_Playing = false;
		m_FrameIndex = 0;
		m_TimeRemaining = 0.0f;
		m_CurrentClipName.clear();
	}

	void SpriteAnimator::Draw(float alpha)
	{
		if (!m_Playing || m_CurrentClip == nullptr)
		{
			return;
		}


	}

	void SpriteAnimator::Update(float dt)
	{
		if (!m_Playing || m_CurrentClip == nullptr)
		{
			return;
		}

		AdvanceFrame(dt);
	}

	void SpriteAnimator::AdvanceFrame(float dt)
	{
		m_TimeRemaining -= dt;

		while (m_TimeRemaining <= 0.0f)
		{
			m_FrameIndex++;

			if (m_FrameIndex >= m_CurrentClip->frames.size())
			{
				if (m_CurrentClip->loop)
				{
					m_FrameIndex = 0;
				}
				else
				{
					Stop();
					return;
				}
			}

			const SpriteFrame& frame = m_CurrentClip->frames[m_FrameIndex];
			m_TimeRemaining += frame.duration;
		}
	}

	const SpriteFrame* SpriteAnimator::GetCurrentFrame() const
	{
		if (!m_Playing || m_CurrentClip == nullptr || m_FrameIndex >= m_CurrentClip->frames.size())
		{
			return nullptr;
		}

		return &m_CurrentClip->frames[m_FrameIndex];
	}

	json SpriteAnimator::Serialize() const
	{
		json doc;

		// Serialize clips
		json clips;
		for (const auto& [name, clip] : m_Clips)
		{
			json clipDoc;
			clipDoc["name"] = clip.name;
			clipDoc["loop"] = clip.loop;

			json frames;
			for (const auto& frame : clip.frames)
			{
				json frameDoc;
				frameDoc["texturePath"] = ""; // Would need to track original path
				frameDoc["sourceRect"] = {
					{"x", frame.sourceRect.x},
					{"y", frame.sourceRect.y},
					{"w", frame.sourceRect.w},
					{"h", frame.sourceRect.h}
				};
				frameDoc["duration"] = frame.duration;
				frames.push_back(frameDoc);
			}
			clipDoc["frames"] = frames;
			clips[name] = clipDoc;
		}
		doc["clips"] = clips;

		// Serialize state
		doc["playing"] = m_Playing;
		doc["currentClip"] = m_CurrentClipName;
		doc["frameIndex"] = m_FrameIndex;

		return doc;
	}

	void SpriteAnimator::Deserialize(const json& doc)
	{
		m_Clips.clear();

		// Deserialize clips
		if (doc.contains("clips"))
		{
			for (auto& [name, clipDoc] : doc["clips"].items())
			{
				AnimationClip clip;
				clip.name = clipDoc.value("name", name);
				clip.loop = clipDoc.value("loop", true);

				if (clipDoc.contains("frames"))
				{
					for (const auto& frameDoc : clipDoc["frames"])
					{
						SpriteFrame frame;
						// texturePath would need to be resolved via TextureManager
						frame.sourceRect.x = frameDoc["sourceRect"]["x"];
						frame.sourceRect.y = frameDoc["sourceRect"]["y"];
						frame.sourceRect.w = frameDoc["sourceRect"]["w"];
						frame.sourceRect.h = frameDoc["sourceRect"]["h"];
						frame.duration = frameDoc.value("duration", 0.1f);
						clip.frames.push_back(frame);
					}
				}

				m_Clips[name] = clip;
			}
		}

		// Restore state
		m_Playing = doc.value("playing", false);
		m_CurrentClipName = doc.value("currentClip", "");
		m_FrameIndex = doc.value("frameIndex", 0);
	}

	Animation::Animation(Entity* _entity)
		: Component(_entity)
		, m_StateMachine(AnimationStateMachine(&m_Animator))
	{
	}

	void Animation::Start()
	{
	}

	void Animation::Destroy()
	{
		m_StateMachine.ChangeState(""); // clear state
		m_Animator.Stop();
		m_ClipLibrary.Clear();
	}

	void buki::Animation::Draw(float alpha)
	{
		const SpriteFrame* frame = GetCurrentFrame();
		if (!frame || !frame->texture)
		{
			return;
		}

		Transform* transform = m_Entity->T();
		if (transform == nullptr)
		{
			return;
		}

		Color tint{};
		tint.r = 1.0f;
		tint.g = 1.0f;
		tint.b = 1.0f;
		tint.a = 1.0f;


		bool flipX = params.Get("flipx", frame->flipX);
		bool flipY = params.Get("flipy", frame->flipY);

		Engine::Get().Graphics().DrawSprite(
			*frame->texture,
			Engine::Get().GetActiveCamera(),
			{
				transform->position.x,
				transform->position.y
			},
			transform->GetSize().x,
			transform->GetSize().y,
			frame->sourceRect,
			transform->GetRotation().GetRadians(),
			flipX,
			flipY,
			tint
		);
	}

	void Animation::Update(float dt)
	{
		m_StateMachine.Update();
		m_Animator.Update(dt);
	}


	// Animation component serialization
	json Animation::Serialize()
	{
		json doc;
		doc["currentState"] = m_StateMachine.GetCurrentState();
		doc["playing"] = m_Animator.IsPlaying();
		doc["clipName"] = m_Animator.GetCurrentClipName();
		return doc;
	}

	void Animation::Deserialize(json doc)
	{
		//m_Animator.Deserialize(doc);
		// Restore state machine
		std::string stateName = doc.value("currentState", "idle");
		m_StateMachine.ChangeState(stateName);
	}

	void Animation::Set()
	{
		Start();
	}
}
