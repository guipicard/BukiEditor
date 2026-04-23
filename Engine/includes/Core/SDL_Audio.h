#pragma once

#include "IAudio.h"

#include <string>
#include <unordered_map>

struct MIX_Mixer;
struct MIX_Audio;
struct MIX_Track;

namespace buki
{
	class SDL_Audio : public IAudio
	{
	public:
		SDL_Audio();
		~SDL_Audio() override;

		virtual size_t LoadMusic(const std::string& filename) override;
		virtual size_t LoadSound(const std::string& filename) override;

		virtual void PlayMusic(size_t id) override;
		virtual void PlayMusic(size_t id, int loop) override;

		virtual void PlaySFX(size_t id) override;
		virtual void PlaySFX(size_t id, int loop) override;
		virtual void PauseMusic() override;
		virtual void StopMusic() override;
		virtual void ResumeMusic() override;

		virtual void SetVolume(int volume) override;
		virtual void SetVolume(size_t soundId, int volume) override;

		virtual void Destroy() override;

	private:
		static constexpr size_t INVALID_AUDIO_ID = 0;
		static constexpr int MAX_VOLUME = 128;

		bool m_Initialized = false;
		MIX_Mixer* m_Mixer = nullptr;
		MIX_Track* m_MusicTrack = nullptr;

		std::unordered_map<size_t, MIX_Audio*> m_MusicCache;
		std::unordered_map<size_t, MIX_Audio*> m_SfxCache;

		size_t MakeId(const std::string& filename) const;
		int ToSDLVolume(int volume) const;
		float ToGain(int volume) const;

		MIX_Audio* FindMusic(size_t id) const;
		MIX_Audio* FindSFX(size_t id) const;
	};
}