#pragma once

#include "AnimationAsset.h"
#include "ImGui.h"
#include <string>

namespace buki
{
	class AnimationStateMachineEditorWindow
	{
	public:
		void Open(const std::string& animationAssetPath);
		void Close();
		void Draw();

		[[nodiscard]] bool IsOpen() const { return m_Open; }
		[[nodiscard]] bool ShouldDockNextToScene() const
		{
			return m_RequestDockNextToScene;
		}
	private:
		void DrawToolbar();
		void DrawGraph();
		void DrawInspector();

		void DrawStateNode(AnimationState& state);
		void DrawTransitionLines();
		void TrySelectTransition(const ImVec2& mousePos);
		void EnsureLayoutEntries();

		[[nodiscard]] AnimationState* FindState(const std::string& name);
		[[nodiscard]] AnimationStateLayout* FindLayout(const std::string& name);
		[[nodiscard]] ImVec2 GetStatePosition(const std::string& name) const;
		[[nodiscard]] ImVec2 GetStateCenter(const std::string& name) const;
		[[nodiscard]] int GetStateIndex(const std::string& name) const;
		bool IsPointNearCurve(const ImVec2& point, const ImVec2& from, const ImVec2& control, const ImVec2& to, float maxDistance) const;
		ImVec2 GetTransitionControlPoint(const std::string& fromStateName, const std::string& toStateName) const;
		void GetTransitionEndpoints(const std::string& fromStateName, const std::string& toStateName, ImVec2& outFrom, ImVec2& outTo) const;

		std::string m_AssetPath;
		AnimationAsset m_Asset;
		bool m_Open = false;
		bool m_RequestDockNextToScene = false;

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

		ImVec2 m_CanvasOrigin = ImVec2(0.0f, 0.0f);
	};
}