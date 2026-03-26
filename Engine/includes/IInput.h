#pragma once
#include <EKey.h>

namespace buki
{
    struct IInput
    {
        virtual ~IInput() = default; // Destructeur virtuel par défaut.

        virtual bool IsKeyPressed(EKey key) = 0; // Vérifie si la touche spécifiée (key) est actuellement pressée.

        virtual bool IsKeyDown(EKey key) = 0; // Vérifie si la touche spécifiée (key) est en position basse (activée).

        virtual bool IsKeyUp(EKey key) = 0; // Vérifie si la touche spécifiée (key) est en position haute (désactivée).

        virtual bool IsButtonPressed(int button) = 0; // Vérifie si le bouton de la souris spécifié (button) est actuellement pressé.

        virtual bool IsButtonDown(int button) = 0; // Vérifie si le bouton de la souris spécifié (button) est en position basse (activé).

        virtual bool IsButtonUp(int button) = 0; // Vérifie si le bouton de la souris spécifié (button) est en position haute (désactivé).

        virtual void GetMousePosition(float* x, float* y) = 0; // Récupère la position actuelle de la souris, en enregistrant les coordonnées dans x et y.

        virtual void GetMousePositionScreen(int* x, int* y) = 0; // Récupère la position actuelle de la souris, en enregistrant les coordonnées dans x et y.

		virtual bool GetMouseScrollUp(int* amount) = 0; 

		virtual bool GetMouseScrollDown(int* amount) = 0;

        virtual void ResetLateInputs() = 0; // Réinitialise les états des entrées qui ont été enregistrées tardivement.

        virtual void ExitProgram() = 0; // Exits Program

    protected:
        friend class Engine; // Donne un accès spécifique à la classe Engine pour utiliser les méthodes protégées.

        virtual void Update() = 0; // Met à jour l'état des entrées. Cette méthode est probablement appelée à chaque cycle de la boucle principale du jeu.

        bool m_IsRunning = true; // Indique si le gestionnaire d'entrée est actif.
    };
}
