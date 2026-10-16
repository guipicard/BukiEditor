#pragma once

#include <string>

#include <nlohmann/json_fwd.hpp>

namespace buki
{
    class Component;

    struct ComponentPrefab
    {
        int version = 1;
        std::string componentTypeName;

        bool LoadFromFile(const std::string& path);
        bool SaveToFile(const std::string& path) const;

        bool LoadIntoComponent(Component* component) const;
        bool SaveFromComponent(const Component* component);

    private:
        std::string m_SerializedData;
    };
}