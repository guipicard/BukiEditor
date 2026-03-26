#include "Animation.h"

using json = nlohmann::json;	

buki::Animation::Animation(Entity* _entity)
	: Sprite(_entity)
{
}

void buki::Animation::Update(float dt)
{
	if (m_Playing)
	{
		m_Elapsed += dt;
		if (m_Elapsed >= m_Clips[m_CurrentClip].delay)
		{
			m_Elapsed = 0.0f;

			m_CurrentFrame++;
			AnimationClip _playingClip = m_Clips[m_CurrentClip];
			if (m_CurrentFrame == m_LastFrame) 
			{
				if (!m_Loop)
				{
					Stop();
				}
				else
				{
					m_CurrentFrame = m_FirstFrame;
				}
			}
			UpdateFrame();
		}
	}
}

json buki::Animation::Serialize()
{
	json doc;
	return doc;
}

void buki::Animation::Deserialize(json _doc)
{
}

void buki::Animation::Set()
{
}

void buki::Animation::Init(int start, int frameWidth, int frameHeight)
{
	m_FrameWidth = frameWidth;
	m_FrameHeight = frameHeight;

	m_CurrentFrame = start;

	m_Src.x = start * m_FrameWidth;
	m_Src.y = 0;
	m_Src.w = m_FrameWidth;
	m_Src.h = m_FrameHeight;

	m_CurrentClip.clear();
}

void buki::Animation::AddClip(const std::string& name, int startX, int startY, int count, float delay, std::string sprite)
{
	m_Clips.emplace(name, AnimationClip{ startX, startY, count, delay, sprite });
	m_Src.x = startX * m_FrameWidth;
	m_Src.y = startY * m_FrameHeight;
}

void buki::Animation::Stop()
{
	m_Playing = false;
	m_Loop = false;
}

void buki::Animation::Play(const std::string& name, bool loop)
{
	if (name != m_CurrentClip)
	{
		m_CurrentClip = name;
		const AnimationClip _clip = m_Clips[name];
		Load(_clip.sprite);
		m_CurrentFrame = _clip.startX;
		m_FirstFrame = _clip.startX;
		m_LastFrame = _clip.count - _clip.startX - 1;
		//m_LastFrame = _clip.startX + _clip.count - 1;
		m_Delay = _clip.delay;
		m_Loop = loop;
		m_Playing = true;
		UpdateFrame();
	}
}

void buki::Animation::UpdateFrame()
{
	const int _row = m_Clips[m_CurrentClip].startY;
	const int _col = m_CurrentFrame;
	const int _x = m_FrameWidth * _col;
	const int _y = m_FrameHeight * _row;

	m_Src.x = _x;
	m_Src.y = _y;
	m_Src.w = m_FrameWidth;
	m_Src.h = m_FrameHeight;
}
