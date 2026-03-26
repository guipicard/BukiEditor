#pragma once
#include <Sprite.h>
#include <map>
#include <string>
#include <IUpdatable.h>
#include <RectI.h>
#include <RectF.h>
#include "nlohmann/json.hpp"

namespace buki
{
	using json = nlohmann::json;
	struct Entity;
	struct AnimationClip
	{
		int startX;
		int startY;
		int count;
		float delay;
		std::string sprite;
	};

	struct Animation : public Sprite, public IUpdatable
	{
		virtual ~Animation() = default;
		Animation(Entity* _entity);
		virtual void Update(float dt) override;

		virtual json Serialize() override;
		virtual void Deserialize(json _doc) override;
		virtual void Set() override;

		void Init(int start, int frameWidth, int frameHeight);
		void AddClip(const std::string& name, int startX, int startY, int count, float delay, std::string sprite);
		void Stop();
		void Play(const std::string& name, bool loop);
		inline bool IsStopped() const { return !m_Playing; }
		std::string GetPlaying() { return m_CurrentClip; }
	private:
		void UpdateFrame();

		std::map<std::string, AnimationClip> m_Clips;

		int m_FrameWidth = 0;
		int m_FrameHeight = 0;
		int m_FirstFrame = 0;
		int m_LastFrame = 0;
		float m_Delay = 0.0f;
		float m_Elapsed = 0.0f;
		bool m_Playing = false;
		bool m_Loop = false;
		int m_CurrentFrame = 0;
		std::string m_CurrentClip;
	};
}