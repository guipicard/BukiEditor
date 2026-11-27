#include "EngineDebugWindows.h"

#if BUKI_ENABLE_DEBUG_UI

#include "DebugUI.h"
#include "Engine.h"
#include "Entity.h"
#include "Component.h"
#include "ComponentFactory.h"
#include "RigidBody.h"
#include "PropertyWidgets.h"
#include "Units.h"

#include "imgui.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <typeinfo>
#include <vector>

namespace
{
    buki::Entity* s_SelectedEntity = nullptr;

    std::string ToLower(std::string text)
    {
        std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return text;
    }

    void DrawEngineStatsContents()
    {
        buki::Engine& engine = buki::Engine::Get();

        ImGui::Text("FPS: %d", engine.GetFps());
        ImGui::Text("Physics steps/s: %d", engine.GetPps());
        ImGui::Text("Entities: %zu", engine.World().GetEntitiesInWorld().size());

        float timeScale = engine.GetTimeScale();

        if (ImGui::SliderFloat("Time scale", &timeScale, 0.0f, 4.0f))
        {
            engine.SetTimeScale(timeScale);
        }
    }

    void DrawEntityDetails(buki::Entity* entity, bool allowEditing)
    {
        using namespace buki;

        ImGui::TextUnformatted(entity->GetName().c_str());
        ImGui::TextDisabled("Enabled: %s   Z: %d   Layer: '%s'", entity->IsEnabled() ? "yes" : "no", entity->GetZ(), entity->GetLayer().c_str());
        ImGui::Separator();

        const Transform& transform = entity->Tm();
        const Vector2 position = transform.GetPosition();
        const Vector2 size = transform.GetSize();
        const float rotationDegrees = RadiansToClockwiseDegrees(transform.GetRotation().GetRadians());

        ImGui::Text("Position: %.3f, %.3f", position.x, position.y);
        ImGui::Text("Rotation: %.1f deg (clockwise)", rotationDegrees);
        ImGui::Text("Size: %.3f, %.3f", size.x, size.y);

        if (RigidBody* rigidBody = entity->GetComponent<RigidBody>())
        {
            const bool valid = Engine::Get().Physics().IsBodyValid(rigidBody->GetBodyId());
            ImGui::Text("Physics body: %s", valid ? "valid" : "none");
        }

        ImGui::Separator();

        const PropertyEditMode mode = allowEditing ? PropertyEditMode::Editable : PropertyEditMode::ReadOnly;

        for (Component* component : entity->GetComponents())
        {
            if (component == nullptr)
            {
                continue;
            }

            std::string typeName = ComponentFactory::GetTypeName(typeid(*component));

            if (typeName.empty())
            {
                typeName = typeid(*component).name();
            }

            ImGui::PushID(component);

            if (ImGui::TreeNodeEx(typeName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (component->GetProperties().empty())
                {
                    ImGui::TextDisabled("No registered properties");
                }
                else
                {
                    PropertyWidgets::DrawComponentProperties(component, mode);
                }

                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    }

    void DrawWorldInspectorContents()
    {
        using namespace buki;

        static char filterBuffer[128] = "";
        static bool allowEditing = false;

        const std::vector<Entity*>& entities = Engine::Get().World().GetEntitiesInWorld();

        if (s_SelectedEntity != nullptr && std::find(entities.begin(), entities.end(), s_SelectedEntity) == entities.end())
        {
            s_SelectedEntity = nullptr;
        }

        ImGui::SetNextItemWidth(200.0f);
        ImGui::InputTextWithHint("##entityFilter", "Filter entities...", filterBuffer, sizeof(filterBuffer));
        ImGui::SameLine();
        ImGui::Text("%zu entities", entities.size());
        ImGui::SameLine();
        ImGui::Checkbox("Allow editing", &allowEditing);

        if (allowEditing)
        {
            ImGui::TextDisabled("Edits change component data only. Colliders and resources are not rebuilt.");
        }

        ImGui::Separator();

        const std::string filter = ToLower(filterBuffer);

        if (ImGui::BeginChild("##entityList", ImVec2(220.0f, 0.0f), true))
        {
            for (Entity* entity : entities)
            {
                if (entity == nullptr)
                {
                    continue;
                }

                const std::string name = entity->GetName();

                if (!filter.empty() && ToLower(name).find(filter) == std::string::npos)
                {
                    continue;
                }

                ImGui::PushID(entity);

                if (!entity->IsEnabled())
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
                }

                if (ImGui::Selectable(name.c_str(), entity == s_SelectedEntity))
                {
                    s_SelectedEntity = entity;
                }

                if (!entity->IsEnabled())
                {
                    ImGui::PopStyleColor();
                }

                ImGui::PopID();
            }
        }

        ImGui::EndChild();
        ImGui::SameLine();

        if (ImGui::BeginChild("##entityDetails", ImVec2(0.0f, 0.0f), true))
        {
            if (s_SelectedEntity == nullptr)
            {
                ImGui::TextDisabled("Select an entity.");
            }
            else
            {
                DrawEntityDetails(s_SelectedEntity, allowEditing);
            }
        }

        ImGui::EndChild();
    }

    void DrawPhysicsInspectorContents()
    {
        using namespace buki;

        PhysicsService& physics = Engine::Get().Physics();
        const std::vector<Entity*>& entities = Engine::Get().World().GetEntitiesInWorld();

        std::vector<Entity*> bodyOwners;

        for (Entity* entity : entities)
        {
            if (entity == nullptr)
            {
                continue;
            }

            RigidBody* rigidBody = entity->GetComponent<RigidBody>();

            if (rigidBody != nullptr && physics.IsBodyValid(rigidBody->GetBodyId()))
            {
                bodyOwners.push_back(entity);
            }
        }

        PhysicsDebugCounts counts;

        if (physics.GetDebugCounts(counts))
        {
            ImGui::Text("Box2D bodies: %d   shapes: %d   contacts: %d", counts.bodyCount, counts.shapeCount, counts.contactCount);
            ImGui::Text("Bodies owned by world entities: %zu", bodyOwners.size());

            const int unaccounted = counts.bodyCount - static_cast<int>(bodyOwners.size());

            if (unaccounted != 0)
            {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.2f, 1.0f), "%d body(ies) not owned by a live world entity (cached prefab templates, leaked bodies).", unaccounted);
            }
        }
        else
        {
            ImGui::TextDisabled("Physics world unavailable.");
        }

        ImGui::Separator();

        static const char* bodyTypes[] = { "Static", "Kinematic", "Dynamic" };

        if (ImGui::BeginTable("##physicsBodies", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupColumn("Entity");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("Awake");
            ImGui::TableSetupColumn("Mass");
            ImGui::TableSetupColumn("Velocity");
            ImGui::TableHeadersRow();

            for (Entity* entity : bodyOwners)
            {
                const BodyId bodyId = entity->GetComponent<RigidBody>()->GetBodyId();
                const int type = physics.GetType(bodyId);
                const Vector2 velocity = physics.GetVelocity(entity);

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(entity->GetName().c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(type >= 0 && type < 3 ? bodyTypes[type] : "?");

                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(physics.IsAwake(bodyId) ? "yes" : "no");

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%.3f", physics.GetMass(bodyId));

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%.2f, %.2f", velocity.x, velocity.y);
            }

            ImGui::EndTable();
        }
    }
}

void buki::RegisterEngineDebugWindows(DebugUI& ui)
{
    ui.RegisterWindow("engine.stats", DebugMenuGroup::Engine, "Engine Stats", DrawEngineStatsContents);
    ui.RegisterWindow("engine.world", DebugMenuGroup::Engine, "World Inspector", DrawWorldInspectorContents);
    ui.RegisterWindow("engine.physics", DebugMenuGroup::Engine, "Physics Inspector", DrawPhysicsInspectorContents);
}

#endif