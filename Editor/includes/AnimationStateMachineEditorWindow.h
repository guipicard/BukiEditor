#pragma once


#include "AssetEditorWindow.h"
#include "ImGui.h"
#include "AnimationAsset.h"

#include <string>

namespace buki
{
	class Vector2;
	struct EditorState;

	class AnimationStateMachineEditorWindow : public AssetEditorWindow
	{
	public:
		AnimationStateMachineEditorWindow(EditorState& editorState,const std::string& assetPath);
		void Draw() override;
		void Reload() override;

	private:
		bool LoadAsset(const std::string& path) override;
		void OnAssetLoaded() override;
		void OnClosed() override;

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

		bool DoesStateOverlap(const Vector2& position) const;
		Vector2 FindNonOverlappingPosition(float x, float y) const;
		void AddStateAtWorldPosition(float x, float y);

		AnimationAsset m_Asset;

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
		ImVec2 m_CanvasOffset = ImVec2(0.0f, 0.0f);
		bool m_PanningCanvas = false;
		float m_CanvasZoom = 1.0f;
	};
}