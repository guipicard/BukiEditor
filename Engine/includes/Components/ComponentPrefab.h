#pragma once

#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json;


namespace buki
{
    class Component;
    class Entity;

    struct ComponentPrefab
    {
        int version = 1;
        std::string componentTypeName;

        bool LoadFromFile(const std::string& path);
        bool SaveToFile(const std::string& path) const;

        bool LoadFromComponent(const Component* component);
        bool SaveToComponent(Component* component) const;

        bool CreatePreviewComponent(Entity* entity, Component** outComponent) const;

    private:
        json m_ComponentData;
    };
}