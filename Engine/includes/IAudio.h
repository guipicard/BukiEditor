#pragma once
#include <cstdlib>
#include <string>

namespace buki
{
    struct IAudio
    {
        virtual ~IAudio() = default; // Destructeur virtuel par défaut.

        // Charge un fichier de musique et retourne un identifiant pour référencer la musique.
        virtual size_t LoadMusic(const std::string& filename) = 0;

        // Charge un fichier de son (effet sonore) et retourne un identifiant pour le référencer.
        virtual size_t LoadSound(const std::string& filename) = 0;

        // Joue la musique référencée par l'identifiant id.
        virtual void PlayMusic(size_t id) = 0;

        // Joue la musique référencée par l'identifiant id, avec une option pour répéter la musique un certain nombre de fois.
        virtual void PlayMusic(size_t id, int loop) = 0;

        // Joue un effet sonore (SFX) référencé par l'identifiant id.
        virtual void PlaySFX(size_t id) = 0;

        // Joue un effet sonore (SFX) référencé par l'identifiant id, avec une option pour le répéter un certain nombre de fois.
        virtual void PlaySFX(size_t id, int loop) = 0;

        // Met en pause la musique actuellement jouée.
        virtual void PauseMusic() = 0;

        // Arrête la musique actuellement jouée.
        virtual void StopMusic() = 0;

        // Reprend la lecture de la musique qui a été mise en pause.
        virtual void ResumeMusic() = 0;

        // Régle le volume global de la musique.
        virtual void SetVolume(int volume) = 0;

        // Régle le volume d'un son spécifique identifié par soundId.
        virtual void SetVolume(size_t soundId, int volume) = 0;

        // Nettoie et libère les ressources audio.
        virtual void Destroy() = 0;
    };
}
