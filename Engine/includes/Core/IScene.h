#pragma once
#include <string>

namespace buki
{
    class IScene
    {
    public: 
        virtual ~IScene() = default; // Destructeur virtuel par défaut pour une gestion appropriée de la polymorphie.

        virtual bool Load() = 0; // Charge les ressources et initialise la scène. Doit être implémentée pour définir le comportement spécifique au chargement de la scène.

		virtual void SetScene() = 0; // Configure la scène après le chargement, en ajoutant des entités, des composants, etc. Doit être implémentée pour définir la configuration spécifique de la scène.
    
        virtual void OnWindowResize() = 0;

        virtual void OnStart() = 0;

        virtual void OnStop() = 0;
    };
}
