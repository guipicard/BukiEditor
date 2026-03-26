#include "SDL_Audio.h"
#include "SDL_mixer.h"

buki::SDL_Audio::SDL_Audio()
{
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
}

buki::SDL_Audio::~SDL_Audio()
{
}

size_t buki::SDL_Audio::LoadMusic(const std::string& filename)
{
	const size_t id = std::hash<std::string>()(filename);
	if (m_MusicCache.count(id) > 0)
	{
		return id;
	}

	Mix_Music* music;
	music = Mix_LoadMUS(filename.c_str());
	if (music != nullptr)
	{
		m_MusicCache[id] = music;
		return id;
	}
	return -1;
}

size_t buki::SDL_Audio::LoadSound(const std::string& filename)
{
	const size_t id = std::hash<std::string>()(filename.c_str());
	if (m_SfxCache.count(id) > 0)
	{
		return id;
	}

	Mix_Chunk* sample = Mix_LoadWAV(filename.c_str());
	if (sample != nullptr)
	{
		m_SfxCache[id] = sample;
		return id;
	}
	return -1;
}

void buki::SDL_Audio::PlayMusic(size_t id)
{
	Mix_Music* music;
	music = m_MusicCache[id];
	Mix_PlayMusic(music, 0);
}

void buki::SDL_Audio::PlayMusic(size_t id, int loop)
{
	Mix_Music* music;
	music = m_MusicCache[id];
	Mix_PlayMusic(music, loop);
}

void buki::SDL_Audio::PlaySFX(size_t id)
{
	Mix_Chunk* sample;
	sample = m_SfxCache[id];
	Mix_PlayChannel(-1, sample, 0);
}

void buki::SDL_Audio::PlaySFX(size_t id, int loop)
{
	Mix_Chunk* sample;
	sample = m_SfxCache[id];
	Mix_PlayChannel(-1, sample, loop);
}

void buki::SDL_Audio::PauseMusic()
{
	Mix_PauseMusic();
}

void buki::SDL_Audio::StopMusic()
{
	Mix_HaltMusic();
}

void buki::SDL_Audio::ResumeMusic()
{
	Mix_ResumeMusic();
}

void buki::SDL_Audio::SetVolume(int volume)
{
	Mix_Volume(-1, volume);
}

void buki::SDL_Audio::SetVolume(size_t soundId, int volume)
{
	if (m_SfxCache.count(soundId) > 0)
	{
		Mix_Chunk* sample;
		sample = m_SfxCache[soundId];
		Mix_VolumeChunk(sample, volume);
	}
	else if (m_MusicCache.count(soundId) > 0)
	{
		Mix_Music* music;
		music = m_MusicCache[soundId];
		Mix_VolumeMusic(volume);
	}
}

void buki::SDL_Audio::Destroy()
{
	for (auto music : m_MusicCache)
	{
		Mix_FreeMusic(music.second);
	}
	for (auto sfx : m_SfxCache)
	{
		Mix_FreeChunk(sfx.second);
	}
	m_MusicCache.clear();
	m_SfxCache.clear();
	delete& m_MusicCache;
	delete& m_SfxCache;
	Mix_CloseAudio();
}
