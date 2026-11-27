#include "PropertyWidgets.h"

#include "Component.h"
#include "EntityRef.h"
#include "Entity.h"
#include "Button.h"
#include "TileLayer.h"
#include "BukiContainers.h"

#include "imgui.h"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

void* buki::PropertyWidgets::ResolveFieldPointer(const PropertyInfo& info, Component* component)
{
    if (component == nullptr || info.prop.fields.empty())
    {
        return nullptr;
    }

    return reinterpret_cast<char*>(component) + info.prop.fields.back().offset;
}

const char* buki::PropertyWidgets::GetPropertyLabel(const PropertyInfo& info)
{
    return info.prop.fields.empty() ? "" : info.prop.fields.back().key.c_str();
}

bool buki::PropertyWidgets::DrawPropertyValue(const PropertyInfo& info, void* fieldPtr, PropertyEditMode mode)
{
    if (fieldPtr == nullptr || info.prop.fields.empty())
    {
        return false;
    }

    const bool readOnly = mode == PropertyEditMode::ReadOnly;
    const std::string hiddenLabel = "##" + info.prop.fields.back().key;
    const char* id = hiddenLabel.c_str();
    const float speed = info.dragSpeed > 0.0f ? info.dragSpeed : 0.1f;

    bool changed = false;

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

    if (readOnly)
    {
        ImGui::BeginDisabled();
    }

    switch (info.prop.type)
    {
    case PropertyType::Int:
    {
        int* value = static_cast<int*>(fieldPtr);

        if (info.hasMin && info.hasMax)
        {
            changed = ImGui::SliderInt(id, value, static_cast<int>(info.minValue), static_cast<int>(info.maxValue));
        }
        else
        {
            changed = ImGui::DragInt(id, value, speed);

            if (changed && info.hasMin) *value = std::max(*value, static_cast<int>(info.minValue));
            if (changed && info.hasMax) *value = std::min(*value, static_cast<int>(info.maxValue));
        }
        break;
    }

    case PropertyType::Float:
    {
        float* value = static_cast<float*>(fieldPtr);

        if (info.hasMin && info.hasMax)
        {
            changed = ImGui::SliderFloat(id, value, info.minValue, info.maxValue);
        }
        else
        {
            changed = ImGui::DragFloat(id, value, speed, 0.0f, 0.0f, "%.3f");

            if (changed && info.hasMin) *value = std::max(*value, info.minValue);
            if (changed && info.hasMax) *value = std::min(*value, info.maxValue);
        }
        break;
    }

    case PropertyType::Bool:
        changed = ImGui::Checkbox(id, static_cast<bool*>(fieldPtr));
        break;

    case PropertyType::String:
    {
        std::string* value = static_cast<std::string*>(fieldPtr);

        char buffer[512] = {};
        std::snprintf(buffer, sizeof(buffer), "%s", value->c_str());

        if (ImGui::InputText(id, buffer, sizeof(buffer)))
        {
            *value = buffer;
            changed = true;
        }
        break;
    }

    case PropertyType::Vector2:
    {
        Vector2* value = static_cast<Vector2*>(fieldPtr);
        float components[2] = { value->x, value->y };

        if (ImGui::DragFloat2(id, components, speed, 0.0f, 0.0f, "%.3f"))
        {
            value->x = components[0];
            value->y = components[1];
            changed = true;
        }
        break;
    }

    case PropertyType::Color:
    {
        Color* value = static_cast<Color*>(fieldPtr);
        float components[4] = { value->r, value->g, value->b, value->a };

        if (ImGui::ColorEdit4(id, components))
        {
            value->r = components[0];
            value->g = components[1];
            value->b = components[2];
            value->a = components[3];
            changed = true;
        }
        break;
    }

    case PropertyType::RectF:
    {
        RectF* value = static_cast<RectF*>(fieldPtr);
        float components[4] = { value->x, value->y, value->w, value->h };

        if (ImGui::DragFloat4(id, components, speed, 0.0f, 0.0f, "%.3f"))
        {
            value->x = components[0];
            value->y = components[1];
            value->w = components[2];
            value->h = components[3];
            changed = true;
        }
        break;
    }

    case PropertyType::EnumInt:
    {
        int* value = static_cast<int*>(fieldPtr);
        const int count = static_cast<int>(info.enumLabels.size());

        if (count <= 0)
        {
            changed = ImGui::InputInt(id, value);
            break;
        }

        *value = std::clamp(*value, 0, count - 1);

        if (ImGui::BeginCombo(id, info.enumLabels[*value].c_str()))
        {
            for (int i = 0; i < count; ++i)
            {
                const bool selected = *value == i;

                if (ImGui::Selectable(info.enumLabels[i].c_str(), selected))
                {
                    *value = i;
                    changed = true;
                }

                if (selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }
        break;
    }

    case PropertyType::ImageAsset:
    case PropertyType::AudioAsset:
    case PropertyType::FontAsset:
    case PropertyType::AnimationAsset:
    case PropertyType::ClipAsset:
    case PropertyType::PrefabRef:
    {
        const std::string* path = static_cast<const std::string*>(fieldPtr);
        ImGui::TextUnformatted(path->empty() ? "(none)" : path->c_str());
        break;
    }

    case PropertyType::StringList:
    case PropertyType::ImageAssetList:
    case PropertyType::AudioAssetList:
    case PropertyType::FontAssetList:
    {
        const std::vector<std::string>* values = static_cast<const std::vector<std::string>*>(fieldPtr);

        if (ImGui::TreeNode(id, "%zu entries", values->size()))
        {
            for (const std::string& entry : *values)
            {
                ImGui::BulletText("%s", entry.c_str());
            }

            ImGui::TreePop();
        }
        break;
    }

    case PropertyType::EntityRef:
    {
        const EntityRef* ref = static_cast<const EntityRef*>(fieldPtr);

        if (ref->cached != nullptr)
        {
            ImGui::TextUnformatted(ref->cached->GetName().c_str());
        }
        else if (!ref->entityName.empty())
        {
            ImGui::TextUnformatted(ref->entityName.c_str());
        }
        else if (!ref->prefabPath.empty())
        {
            ImGui::TextUnformatted(ref->prefabPath.c_str());
        }
        else
        {
            ImGui::TextUnformatted("(none)");
        }
        break;
    }

    case PropertyType::EnumScriptBinding:
    {
        const StaticFunctionBinding* binding = static_cast<const StaticFunctionBinding*>(fieldPtr);

        if (binding->IsEmpty())
        {
            ImGui::TextUnformatted("(none)");
        }
        else
        {
            ImGui::Text("%s.%s", binding->scriptTypeName.c_str(), binding->functionName.c_str());
        }
        break;
    }

    case PropertyType::TileList:
    {
        const std::vector<TileDrawData>* tiles = static_cast<const std::vector<TileDrawData>*>(fieldPtr);
        ImGui::Text("%zu tiles", tiles->size());
        break;
    }

    default:
        ImGui::TextDisabled("(unsupported type)");
        break;
    }

    if (readOnly)
    {
        ImGui::EndDisabled();
    }

    return changed && !readOnly;
}

bool buki::PropertyWidgets::DrawPropertyRow(const PropertyInfo& info, Component* component, PropertyEditMode mode)
{
    void* fieldPtr = ResolveFieldPointer(info, component);

    if (fieldPtr == nullptr)
    {
        return false;
    }

    ImGui::PushID(fieldPtr);
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(GetPropertyLabel(info));
    ImGui::SameLine(150.0f);

    const bool changed = DrawPropertyValue(info, fieldPtr, mode);

    ImGui::PopID();
    return changed;
}

bool buki::PropertyWidgets::DrawComponentProperties(Component* component, PropertyEditMode mode)
{
    if (component == nullptr)
    {
        return false;
    }

    bool changed = false;
    std::string currentGroup;

    for (const PropertyInfo& info : component->GetProperties())
    {
        if (!info.prop.group.empty() && info.prop.group != currentGroup)
        {
            currentGroup = info.prop.group;
            ImGui::SeparatorText(currentGroup.c_str());
        }

        changed = DrawPropertyRow(info, component, mode) || changed;
    }

    return changed;
}