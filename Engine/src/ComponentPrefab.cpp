#include "ComponentPrefab.h"

#include "Component.h"
#include "Entity.h"
#include "ComponentFactory.h"
#include "Engine.h"

#include <fstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace buki
{
    bool ComponentPrefab::SaveToFile(const std::string& path) const
    {
        json root;
        root["version"] = version;
        root["componentType"] = componentTypeName;
        root["data"] = m_ComponentData;

        std::ofstream file(path);
        if (!file.is_open())
        {
            buki::Engine::Get().Log().LogError("cannot open file: " + path);
            return false;
        }

        file << root.dump(4);
        return true;
    }

    bool ComponentPrefab::LoadFromFile(const std::string& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
            return false;

        json root;

        try
        {
            file >> root;
        }
        catch (...)
        {
            return false;
        }

        version = root.value("version", 1);
        componentTypeName = root.value("componentType", "");

        if (root.contains("data"))
            m_ComponentData = root["data"];
        else
            m_ComponentData = json::object();

        return !componentTypeName.empty();
    }

    bool ComponentPrefab::LoadFromComponent(const Component* component)
    {
        if (!component)
            return false;

        componentTypeName = ComponentFactory::GetTypeName(typeid(component));

        json serialized = component->Serialize();
        if (serialized.is_null())
            return false;

        m_ComponentData = std::move(serialized);
        return true;
    }

    bool ComponentPrefab::SaveToComponent(Component* component) const
    {
        if (!component || m_ComponentData.is_null())
            return false;

        if (ComponentFactory::GetTypeName(typeid(component)) != componentTypeName)
            return false;

        component->Deserialize(m_ComponentData);
        return true;
    }

    bool ComponentPrefab::CreatePreviewComponent(
        Entity* entity,
        Component** outComponent) const
    {
        if (!entity || !outComponent)
            return false;

        *outComponent = nullptr;

        if (componentTypeName.empty())
            return false;

        Component* component =
            ComponentFactory::CreateCmp(entity, componentTypeName, m_ComponentData);

        if (!component)
            return false;

        if (!SaveToComponent(component))
        {
            component->Destroy();
            return false;
        }

        *outComponent = component;
        return true;
    }
}