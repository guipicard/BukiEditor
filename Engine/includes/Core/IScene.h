#pragma once
#include <string>

namespace buki
{
    class Entity;

    class IScene
    {
    public: 
        virtual ~IScene() = default; // Destructeur virtuel par défaut pour une gestion appropriée de la polymorphie.

        virtual void Load() = 0; // Charge les ressources et initialise la scène. Doit être implémentée pour définir le comportement spécifique au chargement de la scène.

		virtual void SetScene() = 0; // Configure la scène après le chargement, en ajoutant des entités, des composants, etc. Doit être implémentée pour définir la configuration spécifique de la scène.

        virtual Entity* Instantiate(const std::string _name) = 0; // Crée et retourne une nouvelle instance de l'entité spécifiée par _name. Cette méthode doit être implémentée pour gérer la création d'entités dans la scène.

        //virtual Entity* Instantiate(const std::string _name) = 0; // Crée et retourne une nouvelle instance de l'entité spécifiée par _name. Cette méthode doit être implémentée pour gérer la création d'entités dans la scène.
    
        virtual void OnWindowResize() = 0;

        virtual void OnStart() = 0;

        virtual void OnStop() = 0;

        virtual void OnNotify(const std::string& button) = 0;
    };
}
