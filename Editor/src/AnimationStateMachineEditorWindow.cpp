#include "AnimationStateMachineEditorWindow.h"
#include "ImGui.h"

namespace buki
{
    namespace
    {
        constexpr ImVec2 NodeSize = ImVec2(170.0f, 64.0f);
        constexpr float ArrowHeadSize = 12.0f;

        ImU32 GetNodeColor(bool selected, bool initial)
        {
            if (selected)
                return IM_COL32(80, 150, 220, 255);

            if (initial)
                return IM_COL32(90, 190, 120, 255);

            return IM_COL32(70, 70, 70, 255);
        }

        void DrawArrowHead(
            ImDrawList* drawList,
            const ImVec2& from,
            const ImVec2& to,
            ImU32 color)
        {
            ImVec2 direction = ImVec2(to.x - from.x, to.y - from.y);
            const float length = sqrtf(
                direction.x * direction.x +
                direction.y * direction.y);

            if (length <= 0.001f)
                return;

            direction.x /= length;
            direction.y /= length;

            const ImVec2 left = ImVec2(
                to.x - direction.x * ArrowHeadSize - direction.y * (ArrowHeadSize * 0.5f),
                to.y - direction.y * ArrowHeadSize + direction.x * (ArrowHeadSize * 0.5f));

            const ImVec2 right = ImVec2(
                to.x - direction.x * ArrowHeadSize + direction.y * (ArrowHeadSize * 0.5f),
                to.y - direction.y * ArrowHeadSize - direction.x * (ArrowHeadSize * 0.5f));

            drawList->AddTriangleFilled(to, left, right, color);
        }
    }

    namespace
    {
        bool IsPointNearSegment(
            const ImVec2& point,
            const ImVec2& a,
            const ImVec2& b,
            float maxDistance)
        {
            const float segmentX = b.x - a.x;
            const float segmentY = b.y - a.y;

            const float lengthSquared =
                segmentX * segmentX + segmentY * segmentY;

            if (lengthSquared <= 0.0001f)
                return false;

            float t =
                ((point.x - a.x) * segmentX +
                    (point.y - a.y) * segmentY) / lengthSquared;

            t = t < 0.0f ? 0.0f : t;
            t = t > 1.0f ? 1.0f : t;

            const float closestX = a.x + segmentX * t;
            const float closestY = a.y + segmentY * t;

            const float deltaX = point.x - closestX;
            const float deltaY = point.y - closestY;

            const float distanceSquared =
                deltaX * deltaX + deltaY * deltaY;

            return distanceSquared <= maxDistance * maxDistance;
        }
    }

    namespace
    {
        const char* GetConditionOpName(buki::AnimConditionOp op)
        {
            using buki::AnimConditionOp;

            switch (op)
            {
            case AnimConditionOp::Equals: return "==";
            case AnimConditionOp::NotEquals: return "!=";
            case AnimConditionOp::Greater: return ">";
            case AnimConditionOp::Less: return "<";
            case AnimConditionOp::GreaterOrEqual: return ">=";
            case AnimConditionOp::LessOrEqual: return "<=";
            }

            return "==";
        }
    }

    void AnimationStateMachineEditorWindow::Open(const std::string& path)
    {
        AnimationAsset loaded;

        if (!loaded.LoadFromFile(path))
            return;

        m_AssetPath = path;
        m_Asset = std::move(loaded);
        m_Open = true;

        m_SelectedStateName.clear();
        m_SelectedTransitionFrom.clear();
        m_SelectedTransitionIndex = -1;

        EnsureLayoutEntries();
    }

    void AnimationStateMachineEditorWindow::Close()
    {
        m_Open = false;
        m_AssetPath.clear();
        m_Asset = AnimationAsset{};

        m_SelectedStateName.clear();
        m_SelectedTransitionFrom.clear();
        m_SelectedTransitionIndex = -1;
    }

    void AnimationStateMachineEditorWindow::Draw()
    {
        if (!m_Open)
            return;

        ImGui::SetNextWindowSize(ImVec2(1100.0f, 700.0f), ImGuiCond_FirstUseEver);

        if (!ImGui::Begin("Animation State Machine", &m_Open))
        {
            ImGui::End();
            return;
        }

        DrawToolbar();
        ImGui::Separator();

        DrawGraph();

        ImGui::Separator();
        DrawInspector();

        ImGui::End();
    }

    void AnimationStateMachineEditorWindow::DrawToolbar()
    {
        if (ImGui::Button("Save"))
        {
            m_Asset.SaveToFile(m_AssetPath);
        }

        ImGui::SameLine();

        if (ImGui::Button("Add State"))
        {
            AnimationState state;
            state.name = "NewState";
            state.clipName = "";

            AnimationStateLayout layout;
            layout.stateName = state.name;
            layout.editorPosition = Vector2(100.0f, 100.0f);

            m_Asset.states.push_back(state);
            m_Asset.layout.push_back(layout);

            m_SelectedStateName = state.name;
        }

        ImGui::SameLine();

        if (ImGui::Button("Add Parameter"))
        {
            AnimParameter parameter;
            parameter.name = "NewParameter";
            parameter.type = AnimParameterType::Float;
            parameter.defaultValue = 0.0f;

            m_Asset.parameters.push_back(parameter);
        }
    }

    void buki::AnimationStateMachineEditorWindow::DrawGraph()
    {
        ImGui::BeginChild("StateMachineCanvas", ImVec2(0.0f, 0.0f), true);

        const ImVec2 mousePos = ImGui::GetIO().MousePos;

        DrawTransitions();

        for (AnimationState& state : m_Asset.states)
        {
            DrawStateNode(state);
        }

        if (m_DraggingTransition)
        {
            DrawPendingTransition(mousePos);

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) &&
                !ImGui::IsAnyItemHovered())
            {
                m_DraggingTransition = false;
                m_PendingTransitionFrom.clear();
            }
        }

        ImGui::EndChild();
    }

    AnimationState* AnimationStateMachineEditorWindow::FindState(
        const std::string& name)
    {
        for (AnimationState& state : m_Asset.states)
        {
            if (state.name == name)
                return &state;
        }

        return nullptr;
    }

    AnimationStateLayout* AnimationStateMachineEditorWindow::FindLayout(
        const std::string& name)
    {
        for (AnimationStateLayout& layout : m_Asset.layout)
        {
            if (layout.stateName == name)
                return &layout;
        }

        return nullptr;
    }

    int AnimationStateMachineEditorWindow::GetStateIndex(
        const std::string& name) const
    {
        for (int index = 0;
            index < static_cast<int>(m_Asset.states.size());
            ++index)
        {
            if (m_Asset.states[index].name == name)
                return index;
        }

        return 0;
    }

    ImVec2 AnimationStateMachineEditorWindow::GetStatePosition(
        const std::string& name) const
    {
        const ImVec2 canvasOrigin = ImGui::GetCursorScreenPos();

        for (const AnimationStateLayout& layout : m_Asset.layout)
        {
            if (layout.stateName == name)
            {
                return ImVec2(
                    canvasOrigin.x + layout.editorPosition.x,
                    canvasOrigin.y + layout.editorPosition.y);
            }
        }

        const int fallbackIndex = GetStateIndex(name);

        return ImVec2(
            canvasOrigin.x + (100.0f + (fallbackIndex % 4) * 220.0f),
            canvasOrigin.y + (100.0f + (fallbackIndex / 4) * 140.0f));
    }

    ImVec2 AnimationStateMachineEditorWindow::GetStateCenter(
        const std::string& name) const
    {
        const ImVec2 position = GetStatePosition(name);

        return ImVec2(
            position.x + NodeSize.x * 0.5f,
            position.y + NodeSize.y * 0.5f);
    }

    void AnimationStateMachineEditorWindow::DrawStateNode(
        AnimationState& state)
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        const ImVec2 nodePosition = GetStatePosition(state.name);
        const ImVec2 nodeEnd = ImVec2(
            nodePosition.x + NodeSize.x,
            nodePosition.y + NodeSize.y);

        ImGui::SetCursorScreenPos(nodePosition);
        ImGui::PushID(state.name.c_str());

        const bool selected = (m_SelectedStateName == state.name);
        const bool initial = (state.name == m_Asset.initialState);

        if (ImGui::Button(state.name.c_str(), NodeSize))
        {
            m_SelectedStateName = state.name;
            m_SelectedTransitionFrom.clear();
            m_SelectedTransitionIndex = -1;
        }

        if (ImGui::IsItemActive() &&
            ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            AnimationStateLayout* layout = FindLayout(state.name);

            if (layout)
            {
                const ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;

                layout->editorPosition.x += mouseDelta.x;
                layout->editorPosition.y += mouseDelta.y;
            }
        }

        drawList->AddRect(
            nodePosition,
            nodeEnd,
            GetNodeColor(selected, initial),
            6.0f,
            0,
            selected || initial ? 2.0f : 1.0f);

        if (m_DraggingTransition &&
            ImGui::IsItemHovered() &&
            ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            FinishTransitionDrag(state.name);
        }

        if (ImGui::BeginPopupContextItem("StateContextMenu"))
        {
            if (ImGui::MenuItem("Add Transition"))
            {
                BeginTransitionDrag(state.name);
            }

            if (ImGui::MenuItem("Set As Initial State"))
            {
                m_Asset.initialState = state.name;
            }

            ImGui::EndPopup();
        }

        ImGui::PopID();
    }

    void buki::AnimationStateMachineEditorWindow::DrawInspector()
    {
        ImGui::BeginChild("Inspector", ImVec2(0.0f, 280.0f), true);

        if (!m_SelectedTransitionFrom.empty())
        {
            DrawSelectedTransitionInspector();
            ImGui::EndChild();
            return;
        }

        if (m_SelectedStateName.empty())
        {
            ImGui::TextDisabled("Select a state or transition.");
            ImGui::EndChild();
            return;
        }

        AnimationState* state = FindState(m_SelectedStateName);

        if (!state)
        {
            ImGui::TextDisabled("Selected state no longer exists.");
            ImGui::EndChild();
            return;
        }

        ImGui::Text("State: %s", state->name.c_str());

        if (ImGui::BeginCombo("Clip", state->clipName.c_str()))
        {
            for (const AnimationClip& clip : m_Asset.clips)
            {
                const bool selected = (clip.name == state->clipName);

                if (ImGui::Selectable(clip.name.c_str(), selected))
                    state->clipName = clip.name;

                if (selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        if (ImGui::Button("Set As Initial State"))
        {
            m_Asset.initialState = state->name;
        }

        ImGui::EndChild();
    }

    void AnimationStateMachineEditorWindow::EnsureLayoutEntries()
    {
        for (const AnimationState& state : m_Asset.states)
        {
            bool found = false;

            for (const AnimationStateLayout& layout : m_Asset.layout)
            {
                if (layout.stateName == state.name)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                AnimationStateLayout layout;
                layout.stateName = state.name;

                const int index = GetStateIndex(state.name);

                layout.editorPosition = Vector2(
                    100.0f + (index % 4) * 220.0f,
                    100.0f + (index / 4) * 140.0f);

                m_Asset.layout.push_back(layout);
            }
        }
    }

    void buki::AnimationStateMachineEditorWindow::DrawTransitions()
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        const ImVec2 mousePos = ImGui::GetIO().MousePos;

        for (const AnimationState& state : m_Asset.states)
        {
            for (int transitionIndex = 0;
                transitionIndex < static_cast<int>(state.transitions.size());
                ++transitionIndex)
            {
                const AnimationTransition& transition =
                    state.transitions[transitionIndex];

                const AnimationState* destination =
                    m_Asset.GetState(transition.to);

                if (!destination)
                    continue;

                const ImVec2 from = GetStateCenter(state.name);
                const ImVec2 to = GetStateCenter(destination->name);

                const bool selected =
                    m_SelectedTransitionFrom == state.name &&
                    m_SelectedTransitionIndex == transitionIndex;

                const ImU32 color = selected
                    ? IM_COL32(255, 210, 120, 255)
                    : IM_COL32(170, 170, 170, 255);

                drawList->AddLine(from, to, color, selected ? 3.0f : 2.0f);
                DrawArrowHead(drawList, from, to, color);

                // Arrow hit-testing.
                if (IsPointNearSegment(mousePos, from, to, 8.0f) &&
                    ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    m_SelectedTransitionFrom = state.name;
                    m_SelectedTransitionIndex = transitionIndex;
                    m_SelectedStateName.clear();
                }
            }
        }
    }

    void buki::AnimationStateMachineEditorWindow::BeginTransitionDrag(
        const std::string& fromStateName)
    {
        m_DraggingTransition = true;
        m_PendingTransitionFrom = fromStateName;
    }

    void buki::AnimationStateMachineEditorWindow::FinishTransitionDrag(
        const std::string& toStateName)
    {
        if (!m_DraggingTransition || m_PendingTransitionFrom.empty())
            return;

        if (toStateName == m_PendingTransitionFrom)
            return;

        AnimationState* fromState = FindState(m_PendingTransitionFrom);

        if (!fromState)
            return;

        AnimationTransition transition;
        transition.to = toStateName;
        transition.forceRestart = false;

        fromState->transitions.push_back(transition);

        m_SelectedTransitionFrom = m_PendingTransitionFrom;
        m_SelectedTransitionIndex =
            static_cast<int>(fromState->transitions.size()) - 1;

        m_DraggingTransition = false;
        m_PendingTransitionFrom.clear();
    }

    void buki::AnimationStateMachineEditorWindow::DrawPendingTransition(
        const ImVec2& mousePos)
    {
        if (m_PendingTransitionFrom.empty())
            return;

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        const ImVec2 from = GetStateCenter(m_PendingTransitionFrom);

        drawList->AddLine(
            from,
            mousePos,
            IM_COL32(255, 210, 120, 255),
            2.0f);
    }


    void buki::AnimationStateMachineEditorWindow::DrawConditionOperatorEditor(
        AnimCondition& condition)
    {
        const char* currentOpName = GetConditionOpName(condition.op);

        if (ImGui::BeginCombo("Operation", currentOpName))
        {
            for (int opIndex = 0;
                opIndex <= static_cast<int>(AnimConditionOp::LessOrEqual);
                ++opIndex)
            {
                const AnimConditionOp op =
                    static_cast<AnimConditionOp>(opIndex);

                const bool isSelected = (condition.op == op);

                if (ImGui::Selectable(GetConditionOpName(op), isSelected))
                    condition.op = op;

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }
    }

    void buki::AnimationStateMachineEditorWindow::DrawConditionValueEditor(
        AnimCondition& condition)
    {
        switch (condition.type)
        {
        case AnimParameterType::Bool:
        {
            bool value = std::get<bool>(condition.value);

            if (ImGui::Checkbox("Value", &value))
                condition.value = value;

            break;
        }

        case AnimParameterType::Int:
        {
            int value = std::get<int>(condition.value);

            if (ImGui::InputInt("Value", &value))
                condition.value = value;

            break;
        }

        case AnimParameterType::Float:
        {
            float value = std::get<float>(condition.value);

            if (ImGui::InputFloat("Value", &value))
                condition.value = value;

            break;
        }

        case AnimParameterType::String:
        {
            char buffer[256];
            std::snprintf(
                buffer,
                sizeof(buffer),
                "%s",
                std::get<std::string>(condition.value).c_str());

            if (ImGui::InputText("Value", buffer, sizeof(buffer)))
                condition.value = std::string(buffer);

            break;
        }
        }
    }

    void buki::AnimationStateMachineEditorWindow::DrawSelectedTransitionInspector()
    {
        AnimationState* fromState = FindState(m_SelectedTransitionFrom);

        if (!fromState ||
            m_SelectedTransitionIndex < 0 ||
            m_SelectedTransitionIndex >=
            static_cast<int>(fromState->transitions.size()))
        {
            ImGui::TextDisabled("Selected transition no longer exists.");
            return;
        }

        AnimationTransition& transition =
            fromState->transitions[m_SelectedTransitionIndex];

        ImGui::Text(
            "Transition: %s -> %s",
            fromState->name.c_str(),
            transition.to.c_str());

        if (ImGui::BeginCombo("To State", transition.to.c_str()))
        {
            for (const AnimationState& state : m_Asset.states)
            {
                const bool selected = (state.name == transition.to);

                if (ImGui::Selectable(state.name.c_str(), selected))
                    transition.to = state.name;

                if (selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        ImGui::Checkbox("Force Restart", &transition.forceRestart);

        ImGui::Separator();

        if (ImGui::Button("Add Condition"))
        {
            AnimCondition condition;

            if (!m_Asset.parameters.empty())
            {
                const AnimParameter& parameter = m_Asset.parameters.front();

                condition.parameter = parameter.name;
                condition.type = parameter.type;
                condition.value = parameter.defaultValue;
            }

            transition.conditions.push_back(condition);
        }

        int conditionToRemove = -1;

        for (int conditionIndex = 0;
            conditionIndex < static_cast<int>(transition.conditions.size());
            ++conditionIndex)
        {
            AnimCondition& condition = transition.conditions[conditionIndex];

            ImGui::PushID(conditionIndex);

            if (ImGui::BeginCombo("Parameter", condition.parameter.c_str()))
            {
                for (const AnimParameter& parameter : m_Asset.parameters)
                {
                    const bool selected =
                        (parameter.name == condition.parameter);

                    if (ImGui::Selectable(parameter.name.c_str(), selected))
                    {
                        condition.parameter = parameter.name;
                        condition.type = parameter.type;
                        condition.value = parameter.defaultValue;
                    }

                    if (selected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            DrawConditionOperatorEditor(condition);
            DrawConditionValueEditor(condition);

            if (ImGui::Button("Remove Condition"))
                conditionToRemove = conditionIndex;

            ImGui::PopID();
        }

        if (conditionToRemove >= 0)
        {
            transition.conditions.erase(
                transition.conditions.begin() + conditionToRemove);
        }

        if (ImGui::Button("Delete Transition"))
        {
            fromState->transitions.erase(
                fromState->transitions.begin() + m_SelectedTransitionIndex);

            m_SelectedTransitionFrom.clear();
            m_SelectedTransitionIndex = -1;
        }
    }
}