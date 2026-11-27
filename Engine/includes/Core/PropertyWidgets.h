#pragma once

#include "PropertyInfo.h"

namespace buki
{
    class Component;

    enum class PropertyEditMode
    {
        ReadOnly,
        Editable
    };

    namespace PropertyWidgets
    {
        void* ResolveFieldPointer(const PropertyInfo& info, Component* component);
        const char* GetPropertyLabel(const PropertyInfo& info);

        bool DrawPropertyValue(const PropertyInfo& info, void* fieldPtr, PropertyEditMode mode);
        bool DrawPropertyRow(const PropertyInfo& info, Component* component, PropertyEditMode mode);
        bool DrawComponentProperties(Component* component, PropertyEditMode mode);
    }
}