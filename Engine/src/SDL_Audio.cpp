#include "SDL_Audio.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <functional>

buki::SDL_Audio::SDL_Audio()
{
    if (!SDL_WasInit(SDL_INIT_AUDIO))
    {
        if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
        {
            return;
        }
    }

    if (!MIX_Init())
    {
        return;
    }

    SDL_AudioSpec spec{};
    spec.format = SDL_AUDIO_F32;
    spec.channels = 2;
    spec.freq = 44100;

    m_Mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    if (m_Mixer == nullptr)
    {
        MIX_Quit();
        return;
    }

    m_MusicTrack = MIX_CreateTrack(m_Mixer);
    if (m_MusicTrack == nullptr)
    {
        MIX_DestroyMixer(m_Mixer);
        m_Mixer = nullptr;
        MIX_Quit();
        return;
    }

    m_Initialized = true;
}

buki::SDL_Audio::~SDL_Audio()
{
    Destroy();
}

size_t buki::SDL_Audio::MakeId(const std::string& filename) const
{
    const size_t id = std::hash<std::string>{}(filename);
    return id == INVALID_AUDIO_ID ? 1 : id;
}

int buki::SDL_Audio::ToSDLVolume(int volume) const
{
    if (volume < 0) return 0;
    if (volume > MAX_VOLUME) return MAX_VOLUME;
    return volume;
}

float buki::SDL_Audio::ToGain(int volume) const
{
    return static_cast<float>(ToSDLVolume(volume)) / static_cast<float>(MAX_VOLUME);
}

MIX_Audio* buki::SDL_Audio::FindMusic(size_t id) const
{
    auto it = m_MusicCache.find(id);
    return it != m_MusicCache.end() ? it->second : nullptr;
}

MIX_Audio* buki::SDL_Audio::FindSFX(size_t id) const
{
    auto it = m_SfxCache.find(id);
    return it != m_SfxCache.end() ? it->second : nullptr;
}

size_t buki::SDL_Audio::LoadMusic(const std::string& filename)
{
    if (!m_Initialized || m_Mixer == nullptr)
    {
        return INVALID_AUDIO_ID;
    }

    const size_t id = MakeId(filename);
    if (m_MusicCache.find(id) != m_MusicCache.end())
    {
        return id;
    }

    MIX_Audio* audio = MIX_LoadAudio(m_Mixer, filename.c_str(), false);
    if (audio == nullptr)
    {
        return INVALID_AUDIO_ID;
    }

    m_MusicCache.emplace(id, audio);
    return id;
}

size_t buki::SDL_Audio::LoadSound(const std::string& filename)
{
    if (!m_Initialized || m_Mixer == nullptr)
    {
        return INVALID_AUDIO_ID;
    }

    const size_t id = MakeId(filename);
    if (m_SfxCache.find(id) != m_SfxCache.end())
    {
        return id;
    }

    MIX_Audio* audio = MIX_LoadAudio(m_Mixer, filename.c_str(), false);
    if (audio == nullptr)
    {
        return INVALID_AUDIO_ID;
    }

    m_SfxCache.emplace(id, audio);
    return id;
}

void buki::SDL_Audio::PlayMusic(size_t id)
{
    PlayMusic(id, 0);
}

void buki::SDL_Audio::PlayMusic(size_t id, int loop)
{
    if (!m_Initialized || m_Mixer == nullptr || m_MusicTrack == nullptr)
    {
        return;
    }

    MIX_Audio* music = FindMusic(id);
    if (music == nullptr)
    {
        return;
    }

    if (!MIX_SetTrackAudio(m_MusicTrack, music))
    {
        return;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    if (props)
    {
        SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loop);
        MIX_PlayTrack(m_MusicTrack, props);
        SDL_DestroyProperties(props);
    }
    else
    {
        MIX_PlayTrack(m_MusicTrack, 0);
    }
}

void buki::SDL_Audio::PlaySFX(size_t id)
{
    PlaySFX(id, 0);
}

void buki::SDL_Audio::PlaySFX(size_t id, int loop)
{
    if (!m_Initialized || m_Mixer == nullptr)
    {
        return;
    }

    MIX_Audio* sfx = FindSFX(id);
    if (sfx == nullptr)
    {
        return;
    }

    if (loop == 0)
    {
        MIX_PlayAudio(m_Mixer, sfx);
        return;
    }

    MIX_Track* track = MIX_CreateTrack(m_Mixer);
    if (track == nullptr)
    {
        return;
    }

    if (!MIX_SetTrackAudio(track, sfx))
    {
        MIX_DestroyTrack(track);
        return;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    if (props)
    {
        SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loop);
        MIX_PlayTrack(track, props);
        SDL_DestroyProperties(props);
    }
    else
    {
        MIX_PlayTrack(track, 0);
    }
}

void buki::SDL_Audio::PauseMusic()
{
    if (m_Initialized && m_MusicTrack != nullptr)
    {
        MIX_PauseTrack(m_MusicTrack);
    }
}

void buki::SDL_Audio::StopMusic()
{
    if (m_Initialized && m_MusicTrack != nullptr)
    {
        MIX_StopTrack(m_MusicTrack, 0);
    }
}

void buki::SDL_Audio::ResumeMusic()
{
    if (m_Initialized && m_MusicTrack != nullptr)
    {
        MIX_ResumeTrack(m_MusicTrack);
    }
}

void buki::SDL_Audio::SetVolume(int volume)
{
    if (!m_Initialized || m_Mixer == nullptr)
    {
        return;
    }
	MIX_SetMixerGain(m_Mixer, ToGain(volume));
}

void buki::SDL_Audio::SetVolume(size_t soundId, int volume)
{
    if (!m_Initialized)
    {
        return;
    }

    const float gain = ToGain(volume);

    if (FindMusic(soundId) != nullptr)
    {
        if (m_MusicTrack != nullptr)
        {
            MIX_SetTrackGain(m_MusicTrack, gain);
        }
        return;
    }
}

void buki::SDL_Audio::Destroy()
{
    if (m_MusicTrack)
    {
        MIX_DestroyTrack(m_MusicTrack);
        m_MusicTrack = nullptr;
    }
    for (auto& pair : m_MusicCache)
    {
        MIX_DestroyAudio(pair.second);
    }
    m_MusicCache.clear();
    for (auto& pair : m_SfxCache)
    {
        MIX_DestroyAudio(pair.second);
    }
    m_SfxCache.clear();
    if (m_Mixer)
    {
        MIX_DestroyMixer(m_Mixer);
        m_Mixer = nullptr;
    }
    if (SDL_WasInit(SDL_INIT_AUDIO))
    {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
    MIX_Quit();
}
