#pragma once

#include "imgui.h"

namespace buki
{
    struct EditorCanvasNavigation
    {
        bool panning = false;

        ImVec2 Update(bool hovered)
        {
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Middle))
            {
                panning = false;
                return ImVec2(0.0f, 0.0f);
            }

            if (!panning &&
                hovered &&
                ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
            {
                panning = true;
            }

            if (!panning)
            {
                return ImVec2(0.0f, 0.0f);
            }

            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

            return ImGui::GetIO().MouseDelta;
        }

        void Reset()
        {
            panning = false;
        }
    };
}