#pragma once
#include <cstdlib>
#include <string>
#include <windows.h>

namespace buki {
    struct ILogger
    {
        virtual ~ILogger() = default; // Destructeur virtuel par défaut pour une gestion appropriée de la polymorphie.

        virtual void LogError(std::string _text) = 0; // Journalise un message d'erreur. _text spécifie le contenu du message d'erreur.

        virtual void LogSdlError() = 0; // Journalise un message d'erreur spécifique à SDL. Cette méthode peut être utilisée pour capturer et afficher les erreurs générées par SDL.

        virtual void LogWarning(std::string _text) = 0; // Journalise un message d'avertissement. _text spécifie le contenu du message d'avertissement.

        virtual void LogSuccess(std::string _text) = 0; // Journalise un message de succès. _text spécifie le contenu du message de succès.

        virtual void LogMessage(std::string _text) = 0; // Journalise un message générique. _text spécifie le contenu du message.

    };
}
