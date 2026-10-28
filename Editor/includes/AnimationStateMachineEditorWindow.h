#pragma once

#include "AnimationAsset.h"
#include <string>

struct ImVec2;
namespace buki
{
    class AnimationStateMachineEditorWindow
    {
    public:
        void Open(const std::string& animationAssetPath);
        void Close();
        void Draw();

        [[nodiscard]] bool IsOpen() const { return m_Open; }

    private:
        void DrawToolbar();
        void DrawGraph();
        void DrawInspector();

        void DrawStateNode(AnimationState& state);
        void DrawTransitions();
        void EnsureLayoutEntries();

        [[nodiscard]] AnimationState* FindState(const std::string& name);
        [[nodiscard]] AnimationStateLayout* FindLayout(const std::string& name);
        [[nodiscard]] ImVec2 GetStatePosition(const std::string& name) const;
        [[nodiscard]] ImVec2 GetStateCenter(const std::string& name) const;
        [[nodiscard]] int GetStateIndex(const std::string& name) const;

        std::string m_AssetPath;
        AnimationAsset m_Asset;
        bool m_Open = false;

        std::string m_SelectedStateName;
        std::string m_SelectedTransitionFrom;
        int m_SelectedTransitionIndex = -1;

        void DrawPendingTransition(const ImVec2& mousePos);
        void DrawSelectedTransitionInspector();
        void DrawConditionOperatorEditor(AnimCondition& condition);
        void DrawConditionValueEditor(AnimCondition& condition);

        bool m_DraggingTransition = false;
        std::string m_PendingTransitionFrom;

        void BeginTransitionDrag(const std::string& fromStateName);
        void FinishTransitionDrag(const std::string& toStateName);
    };
}