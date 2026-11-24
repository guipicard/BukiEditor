#pragma once
#include "AnimationStateMachineEditorWindow.h"
#include "PropertyInfo.h"
#include "InspectorPropertyDrawer.h"
#include "IAssetBehavior.h"
#include "EditorState.h"
#include <cstdio>
#include <string>
#include <utility>
#include <variant>
#include <algorithm>
#include <cctype>
#include <filesystem>
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

	ImVec2 GetBezierPoint(
		const ImVec2& a,
		const ImVec2& control,
		const ImVec2& b,
		float t)
	{
		const float oneMinusT = 1.0f - t;

		const float x =
			oneMinusT * oneMinusT * a.x +
			2.0f * oneMinusT * t * control.x +
			t * t * b.x;

		const float y =
			oneMinusT * oneMinusT * a.y +
			2.0f * oneMinusT * t * control.y +
			t * t * b.y;

		return ImVec2(x, y);
	}

	bool IsPointNearCurve(
		const ImVec2& point,
		const ImVec2& from,
		const ImVec2& control,
		const ImVec2& to,
		float maxDistance)
	{
		constexpr int SampleCount = 24;

		ImVec2 previous = from;

		for (int sample = 1; sample <= SampleCount; ++sample)
		{
			const float t =
				static_cast<float>(sample) / static_cast<float>(SampleCount);

			const ImVec2 current = GetBezierPoint(
				from, control, to, t);

			if (IsPointNearSegment(point, previous, current, maxDistance))
				return true;

			previous = current;
		}

		return false;
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

namespace
{
	ImVec2 GetRectEdgePoint(
		const ImVec2& fromCenter,
		const ImVec2& toCenter,
		const ImVec2& nodeSize,
		float extraPadding)
	{
		float directionX = toCenter.x - fromCenter.x;
		float directionY = toCenter.y - fromCenter.y;

		const float length = sqrtf(
			directionX * directionX +
			directionY * directionY);

		if (length <= 0.001f)
			return fromCenter;

		directionX /= length;
		directionY /= length;

		// Avoid division by zero for vertical/horizontal lines.
		const float absX = fabsf(directionX);
		const float absY = fabsf(directionY);

		float scaleX = 0.0f;
		float scaleY = 0.0f;

		if (absX > 0.0001f)
			scaleX = (nodeSize.x * 0.5f) / absX;

		if (absY > 0.0001f)
			scaleY = (nodeSize.y * 0.5f) / absY;

		// The ray exits the rectangle at the smaller scale factor.
		const float scale = (scaleX > 0.0f && scaleY > 0.0f)
			? (scaleX < scaleY ? scaleX : scaleY)
			: (scaleX > 0.0f ? scaleX : scaleY);

		return ImVec2(
			fromCenter.x + directionX * (scale + extraPadding),
			fromCenter.y + directionY * (scale + extraPadding));
	}
}


namespace buki
{
	AnimationStateMachineEditorWindow::AnimationStateMachineEditorWindow(EditorState& editorState, const std::string& assetPath)
		: AssetEditorWindow(editorState)
	{
		Open(assetPath);
	}

	void AnimationStateMachineEditorWindow::Reload()
	{
		AnimationAsset loaded;

		if (!loaded.LoadFromFile(m_AssetPath))
			return;

		m_Asset = std::move(loaded);
		EnsureLayoutEntries();
	}

	void buki::AnimationStateMachineEditorWindow::Draw()
	{
		if (!m_Open)
			return;

		ImGui::SetNextWindowSize(
			ImVec2(1100.0f, 700.0f),
			ImGuiCond_FirstUseEver);
		if (m_RequestFocus)
		{
			ImGui::SetNextWindowFocus();
			m_RequestFocus = false;
		}

		const std::string title =
			"Animation: " +
			GetAssetFileName() +
			"###" +
			m_AssetPath;
		if (!ImGui::Begin(title.c_str(), &m_Open))
		{
			ImGui::End();
			return;
		}

		DrawToolbar();

		DrawAssetSettings();

		ImGui::Separator();

		DrawGraph();

		ImGui::Separator();
		DrawInspector();

		ImGui::End();

		m_RequestDockNextToScene = false;
	}

	void AnimationStateMachineEditorWindow::OnClosed()
	{
		m_AssetPath.clear();
		m_Asset = AnimationAsset{};

		m_SelectedStateName.clear();
		m_SelectedTransitionFrom.clear();
		m_SelectedTransitionIndex = -1;

		m_StatePendingDeletion.clear();
		m_DraggingTransition = false;
		m_PendingTransitionFrom.clear();

		m_CanvasNavigation.Reset();
		m_PanningCanvas = false;
	}

	bool AnimationStateMachineEditorWindow::LoadAsset(const std::string& path)
	{

		AnimationAsset loaded;

		if (!loaded.LoadFromFile(path))
		{
			return false;
		}

		m_Asset = std::move(loaded);
		return true;
	}

	void AnimationStateMachineEditorWindow::OnAssetLoaded()
	{
		m_RequestDockNextToScene = true;
		ImGui::SetNextWindowFocus();
		m_RequestFocus = false;

		m_SelectedStateName.clear();
		m_SelectedTransitionFrom.clear();
		m_SelectedTransitionIndex = -1;

		m_StatePendingDeletion.clear();
		m_DraggingTransition = false;
		m_PendingTransitionFrom.clear();

		EnsureLayoutEntries();

		m_CanvasNavigation.Reset();
		m_PanningCanvas = false;
	}

	void AnimationStateMachineEditorWindow::DrawToolbar()
	{
		if (ImGui::Button("Save"))
		{
			m_Asset.name = std::filesystem::path(m_AssetPath).stem().string();
			m_Asset.SaveToFile(m_AssetPath);
		}

		ImGui::SameLine();

		if (ImGui::Button("Add State"))
		{
			AddStateAtWorldPosition(100.0f, 100.0f);
		}
	}

	void buki::AnimationStateMachineEditorWindow::DrawGraph()
	{
		constexpr float inspectorHeight = 280.0f;

		ImVec2 canvasSize = ImGui::GetContentRegionAvail();

		canvasSize.y -=
			inspectorHeight +
			ImGui::GetStyle().ItemSpacing.y;

		canvasSize.x = std::max(1.0f, canvasSize.x);
		canvasSize.y = std::max(100.0f, canvasSize.y);

		ImGui::BeginChild(
			"StateMachineCanvas",
			canvasSize,
			true,
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse);

		m_CanvasOrigin = ImGui::GetCursorScreenPos();

		const bool canvasHovered = ImGui::IsWindowHovered();

		const ImVec2 panDelta = m_CanvasNavigation.Update(canvasHovered);

		m_PanningCanvas = m_CanvasNavigation.panning;

		m_CanvasOffset.x += panDelta.x;
		m_CanvasOffset.y += panDelta.y;

		if (canvasHovered &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
			!ImGui::IsAnyItemHovered())
		{
			const ImVec2 mousePosition = ImGui::GetIO().MousePos;

			m_ContextMenuWorldPosition = ImVec2(
				mousePosition.x -
				m_CanvasOrigin.x -
				m_CanvasOffset.x,

				mousePosition.y -
				m_CanvasOrigin.y -
				m_CanvasOffset.y);

			ImGui::OpenPopup("StateMachineCanvasContextMenu");
		}

		if (ImGui::BeginPopup("StateMachineCanvasContextMenu"))
		{
			if (ImGui::MenuItem("Add State Here"))
			{
				AddStateAtWorldPosition(
					m_ContextMenuWorldPosition.x,
					m_ContextMenuWorldPosition.y);
			}

			ImGui::EndPopup();
		}

		m_CanvasOrigin = ImGui::GetCursorScreenPos();

		const ImVec2 mousePos = ImGui::GetIO().MousePos;

		DrawTransitionLines();

		for (AnimationState& state : m_Asset.states)
		{
			DrawStateNode(state);
		}

		if (!m_StatePendingDeletion.empty())
		{
			const std::string stateName = m_StatePendingDeletion;
			m_StatePendingDeletion.clear();

			DeleteState(stateName);
		}

		TrySelectTransition(mousePos);

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

	bool buki::AnimationStateMachineEditorWindow::IsPointNearCurve(
		const ImVec2& point,
		const ImVec2& from,
		const ImVec2& control,
		const ImVec2& to,
		float maxDistance) const
	{
		constexpr int SampleCount = 24;

		ImVec2 previous = from;

		for (int sample = 1; sample <= SampleCount; ++sample)
		{
			const float t =
				static_cast<float>(sample) / static_cast<float>(SampleCount);

			const ImVec2 current = GetBezierPoint(
				from, control, to, t);

			if (IsPointNearSegment(point, previous, current, maxDistance))
				return true;

			previous = current;
		}

		return false;
	}

	ImVec2 AnimationStateMachineEditorWindow::GetTransitionControlPoint(
		const std::string& fromStateName,
		const std::string& toStateName) const
	{
		const ImVec2 from = GetStateCenter(fromStateName);
		const ImVec2 to = GetStateCenter(toStateName);

		float directionX = to.x - from.x;
		float directionY = to.y - from.y;

		const float length = sqrtf(
			directionX * directionX +
			directionY * directionY);

		if (length <= 0.001f)
			return from;

		directionX /= length;
		directionY /= length;

		constexpr float curveOffset = 40.0f;

		return ImVec2(
			(from.x + to.x) * 0.5f - directionY * curveOffset,
			(from.y + to.y) * 0.5f + directionX * curveOffset);
	}

	void buki::AnimationStateMachineEditorWindow::GetTransitionEndpoints(
		const std::string& fromStateName,
		const std::string& toStateName,
		ImVec2& outFrom,
		ImVec2& outTo) const
	{
		const ImVec2 fromCenter = GetStateCenter(fromStateName);
		const ImVec2 toCenter = GetStateCenter(toStateName);

		outFrom = GetRectEdgePoint(
			fromCenter,
			toCenter,
			NodeSize,
			6.0f);

		outTo = GetRectEdgePoint(
			toCenter,
			fromCenter,
			NodeSize,
			6.0f);
	}

	bool buki::AnimationStateMachineEditorWindow::DoesStateOverlap(
		const Vector2& position) const
	{
		constexpr float Padding = 20.0f;

		for (const AnimationStateLayout& layout : m_Asset.layout)
		{
			const float deltaX =
				fabsf(layout.editorPosition.x - position.x);

			const float deltaY =
				fabsf(layout.editorPosition.y - position.y);

			if (deltaX < NodeSize.x + Padding &&
				deltaY < NodeSize.y + Padding)
			{
				return true;
			}
		}

		return false;
	}

	Vector2 buki::AnimationStateMachineEditorWindow::
		FindNonOverlappingPosition(float x, float y) const
	{
		Vector2 position(x, y);

		if (!DoesStateOverlap(position))
			return position;

		constexpr float StepX = NodeSize.x + 40.0f;
		constexpr float StepY = NodeSize.y + 40.0f;

		for (int row = 0; row < 20; ++row)
		{
			for (int column = 0; column < 20; ++column)
			{
				Vector2 candidate(
					x + static_cast<float>(column) * StepX,
					y + static_cast<float>(row) * StepY);

				if (!DoesStateOverlap(candidate))
					return candidate;
			}
		}

		return position;
	}

	void buki::AnimationStateMachineEditorWindow::
		AddStateAtWorldPosition(float x, float y)
	{
		AnimationState state;
		state.name = "NewState";
		state.clipPath.clear();

		int suffix = 1;

		while (FindState(state.name) != nullptr)
		{
			state.name =
				"NewState" + std::to_string(suffix++);
		}

		AnimationStateLayout layout;
		layout.stateName = state.name;
		layout.editorPosition =
			FindNonOverlappingPosition(x, y);

		m_Asset.states.push_back(state);
		m_Asset.layout.push_back(layout);

		m_SelectedStateName = state.name;
		m_SelectedTransitionFrom.clear();
		m_SelectedTransitionIndex = -1;
	}

	ImVec2 buki::AnimationStateMachineEditorWindow::GetStatePosition(
		const std::string& name) const
	{
		for (const AnimationStateLayout& layout : m_Asset.layout)
		{
			if (layout.stateName == name)
			{
				return ImVec2(
					m_CanvasOrigin.x +
					layout.editorPosition.x +
					m_CanvasOffset.x,

					m_CanvasOrigin.y +
					layout.editorPosition.y +
					m_CanvasOffset.y);
			}
		}

		const int fallbackIndex = GetStateIndex(name);

		return ImVec2(
			m_CanvasOrigin.x + m_CanvasOffset.x +
			(100.0f + (fallbackIndex % 4) * 220.0f),

			m_CanvasOrigin.y + m_CanvasOffset.y +
			(100.0f + (fallbackIndex / 4) * 140.0f));
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

		if (!m_PanningCanvas &&
			ImGui::IsItemActive() &&
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

			ImGui::Separator();

			if (ImGui::MenuItem("Delete State"))
			{
				m_StatePendingDeletion = state.name;
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

		char stateNameBuffer[512];
		std::snprintf(
			stateNameBuffer,
			sizeof(stateNameBuffer),
			"%s",
			state->name.c_str());

		ImGui::TextUnformatted("State Name");

		ImGui::InputText(
			"##StateName",
			stateNameBuffer,
			sizeof(stateNameBuffer));

		const bool finishedEditing =
			ImGui::IsItemDeactivatedAfterEdit();

		const std::string proposedName = stateNameBuffer;
		const std::string oldName = state->name;

		const bool onlyWhitespace =
			proposedName.find_first_not_of(" \t\r\n") ==
			std::string::npos;

		const bool duplicateName =
			proposedName != oldName &&
			FindState(proposedName) != nullptr;

		if (onlyWhitespace)
		{
			ImGui::TextDisabled("State name cannot be empty.");
		}
		else if (duplicateName)
		{
			ImGui::TextDisabled(
				"Another state already uses this name.");
		}

		if (finishedEditing &&
			!onlyWhitespace &&
			!duplicateName)
		{
			RenameState(oldName, proposedName);
		}

		PropertyInfo clipPathProperty = BUKI_PROP_STRING(AnimationState, clipPath);

		const std::string oldClipPath = state->clipPath;

		if (InspectorPropertyDrawer::DrawStringAssetPicker(clipPathProperty, &state->clipPath, "CLIP"))
		{
			if (IAssetBehavior* behavior = m_EditorState.assetBehaviors.Find(AssetType::Anim))
			{
				behavior->OnReferenceChanged(m_EditorState.assetDatabase, m_AssetPath, oldClipPath, state->clipPath);
			}

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

	void buki::AnimationStateMachineEditorWindow::DrawTransitionLines()
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();

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

				ImVec2 from;
				ImVec2 to;

				GetTransitionEndpoints(state.name, destination->name, from, to);

				// Perpendicular offset
				float directionX = to.x - from.x;
				float directionY = to.y - from.y;

				const float length = sqrtf(
					directionX * directionX +
					directionY * directionY);

				if (length <= 0.001f)
					continue;

				directionX /= length;
				directionY /= length;

				const float perpendicularX = -directionY;
				const float perpendicularY = directionX;

				constexpr float curveOffset = 40.0f;

				const ImVec2 control = GetTransitionControlPoint(
					state.name,
					destination->name);

				const bool selected =
					m_SelectedTransitionFrom == state.name &&
					m_SelectedTransitionIndex == transitionIndex;

				const ImU32 color = selected
					? IM_COL32(255, 210, 120, 255)
					: IM_COL32(170, 170, 170, 255);

				drawList->AddBezierQuadratic(
					from,
					control,
					to,
					color,
					selected ? 3.0f : 2.0f,
					0);

				// Arrowhead should point along the curve near the destination.
				const ImVec2 curveEnd = GetBezierPoint(
					from, control, to, 0.9f);

				const ImVec2 curveNearEnd = GetBezierPoint(
					from, control, to, 0.8f);

				DrawArrowHead(
					drawList,
					curveNearEnd,
					curveEnd,
					color);
			}
		}
	}

	void buki::AnimationStateMachineEditorWindow::TrySelectTransition(
		const ImVec2& mousePos)
	{
		if (!ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			return;

		if (ImGui::IsAnyItemHovered())
			return;

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

				ImVec2 from = GetStateCenter(state.name);
				ImVec2 to = GetStateCenter(destination->name);

				GetTransitionEndpoints(state.name, destination->name, from, to);

				float directionX = to.x - from.x;
				float directionY = to.y - from.y;

				const float length = sqrtf(
					directionX * directionX +
					directionY * directionY);

				if (length <= 0.001f)
					continue;

				directionX /= length;
				directionY /= length;

				const ImVec2 control = GetTransitionControlPoint(
					state.name,
					destination->name);

				if (IsPointNearCurve(mousePos, from, control, to, 10.0f))
				{
					m_SelectedTransitionFrom = state.name;
					m_SelectedTransitionIndex = transitionIndex;
					m_SelectedStateName.clear();
					return;
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

	void AnimationStateMachineEditorWindow::DrawAssetSettings()
	{
		if (!ImGui::CollapsingHeader("Animation Settings"))
		{
			return;
		}

		const std::string fileName =
			std::filesystem::path(m_AssetPath).stem().string();

		ImGui::Text("Name: %s", fileName.c_str());

		const char* initialStatePreview =
			m_Asset.initialState.empty()
			? "None"
			: m_Asset.initialState.c_str();

		if (ImGui::BeginCombo(
			"Initial State",
			initialStatePreview))
		{
			if (ImGui::Selectable(
				"None",
				m_Asset.initialState.empty()))
			{
				m_Asset.initialState.clear();
			}

			for (const AnimationState& state : m_Asset.states)
			{
				const bool selected =
					state.name == m_Asset.initialState;

				if (ImGui::Selectable(
					state.name.c_str(),
					selected))
				{
					m_Asset.initialState = state.name;
				}

				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		DrawParametersSection();
	}

	void AnimationStateMachineEditorWindow::DrawParametersSection()
	{
		ImGui::SeparatorText("Parameters");

		auto parameterNameExists =
			[&](const std::string& name, int excludedIndex)
			{
				for (int index = 0;
					index < static_cast<int>(m_Asset.parameters.size());
					++index)
				{
					if (index == excludedIndex)
					{
						continue;
					}

					if (m_Asset.parameters[index].name == name)
					{
						return true;
					}
				}

				return false;
			};

		if (ImGui::Button("Add Parameter"))
		{
			AnimParameter parameter;
			parameter.name = "NewParameter";
			parameter.type = AnimParameterType::Float;
			parameter.defaultValue = 0.0f;

			int suffix = 1;

			while (parameterNameExists(parameter.name, -1))
			{
				parameter.name =
					"NewParameter" + std::to_string(suffix++);
			}

			m_Asset.parameters.push_back(std::move(parameter));
		}

		if (m_Asset.parameters.empty())
		{
			ImGui::TextDisabled(
				"No parameters. Add one to use in transitions.");

			return;
		}

		int parameterToRemove = -1;

		for (int parameterIndex = 0;
			parameterIndex <
			static_cast<int>(m_Asset.parameters.size());
			++parameterIndex)
		{
			AnimParameter& parameter =
				m_Asset.parameters[parameterIndex];

			ImGui::PushID(parameterIndex);

			const std::string label =
				parameter.name + "###Parameter";

			if (ImGui::TreeNode(label.c_str()))
			{
				char nameBuffer[512];
				std::snprintf(
					nameBuffer,
					sizeof(nameBuffer),
					"%s",
					parameter.name.c_str());

				if (ImGui::InputText(
					"Name",
					nameBuffer,
					sizeof(nameBuffer),
					ImGuiInputTextFlags_EnterReturnsTrue))
				{
					const std::string newName = nameBuffer;

					if (!newName.empty() &&
						!parameterNameExists(
							newName,
							parameterIndex))
					{
						const std::string oldName =
							parameter.name;

						parameter.name = newName;

						for (AnimationState& state :
							m_Asset.states)
						{
							for (AnimationTransition& transition :
								state.transitions)
							{
								for (AnimCondition& condition :
									transition.conditions)
								{
									if (condition.parameter == oldName)
									{
										condition.parameter = newName;
									}
								}
							}
						}
					}
				}

				ImGui::TextDisabled(
					"Press Enter to commit a unique parameter name.");

				int typeIndex =
					static_cast<int>(parameter.type);

				const char* typeNames[] =
				{
					"Bool",
					"Int",
					"Float",
					"String"
				};

				const bool typeChanged = ImGui::Combo(
					"Type",
					&typeIndex,
					typeNames,
					IM_ARRAYSIZE(typeNames));

				if (typeChanged)
				{
					parameter.type =
						static_cast<AnimParameterType>(typeIndex);

					switch (parameter.type)
					{
					case AnimParameterType::Bool:
						parameter.defaultValue = false;
						break;

					case AnimParameterType::Int:
						parameter.defaultValue = 0;
						break;

					case AnimParameterType::Float:
						parameter.defaultValue = 0.0f;
						break;

					case AnimParameterType::String:
						parameter.defaultValue = std::string{};
						break;
					}
				}

				switch (parameter.type)
				{
				case AnimParameterType::Bool:
				{
					bool value =
						std::get<bool>(parameter.defaultValue);

					if (ImGui::Checkbox("Default", &value))
					{
						parameter.defaultValue = value;
					}

					break;
				}

				case AnimParameterType::Int:
				{
					int value =
						std::get<int>(parameter.defaultValue);

					if (ImGui::InputInt("Default", &value))
					{
						parameter.defaultValue = value;
					}

					break;
				}

				case AnimParameterType::Float:
				{
					float value =
						std::get<float>(parameter.defaultValue);

					if (ImGui::InputFloat("Default", &value))
					{
						parameter.defaultValue = value;
					}

					break;
				}

				case AnimParameterType::String:
				{
					char buffer[512];
					std::snprintf(
						buffer,
						sizeof(buffer),
						"%s",
						std::get<std::string>(
							parameter.defaultValue).c_str());

					if (ImGui::InputText(
						"Default",
						buffer,
						sizeof(buffer)))
					{
						parameter.defaultValue =
							std::string(buffer);
					}

					break;
				}
				}

				if (typeChanged)
				{
					// Keep condition variants consistent with the new type.
					// Existing comparison values are reset to the new default.
					for (AnimationState& state : m_Asset.states)
					{
						for (AnimationTransition& transition :
							state.transitions)
						{
							for (AnimCondition& condition :
								transition.conditions)
							{
								if (condition.parameter ==
									parameter.name)
								{
									condition.type = parameter.type;
									condition.value =
										parameter.defaultValue;
								}
							}
						}
					}
				}

				bool referenced = false;

				for (const AnimationState& state : m_Asset.states)
				{
					for (const AnimationTransition& transition :
						state.transitions)
					{
						for (const AnimCondition& condition :
							transition.conditions)
						{
							if (condition.parameter == parameter.name)
							{
								referenced = true;
							}
						}
					}
				}

				ImGui::BeginDisabled(referenced);

				if (ImGui::Button("Remove Parameter"))
				{
					parameterToRemove = parameterIndex;
				}

				ImGui::EndDisabled();

				if (referenced)
				{
					ImGui::TextDisabled(
						"Remove or reassign conditions using this "
						"parameter before deleting it.");
				}

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		if (parameterToRemove >= 0)
		{
			m_Asset.parameters.erase(
				m_Asset.parameters.begin() + parameterToRemove);
		}
	}

	bool buki::AnimationStateMachineEditorWindow::RenameState(
		const std::string& oldName,
		const std::string& newName)
	{
		if (oldName == newName)
		{
			return true;
		}

		if (newName.find_first_not_of(" \t\r\n") ==
			std::string::npos)
		{
			return false;
		}

		AnimationState* state = FindState(oldName);

		if (state == nullptr || FindState(newName) != nullptr)
		{
			return false;
		}

		state->name = newName;

		for (AnimationStateLayout& layout : m_Asset.layout)
		{
			if (layout.stateName == oldName)
			{
				layout.stateName = newName;
			}
		}

		for (AnimationState& otherState : m_Asset.states)
		{
			for (AnimationTransition& transition :
				otherState.transitions)
			{
				if (transition.to == oldName)
				{
					transition.to = newName;
				}
			}
		}

		if (m_Asset.initialState == oldName)
		{
			m_Asset.initialState = newName;
		}

		if (m_SelectedStateName == oldName)
		{
			m_SelectedStateName = newName;
		}

		if (m_SelectedTransitionFrom == oldName)
		{
			m_SelectedTransitionFrom = newName;
		}

		if (m_PendingTransitionFrom == oldName)
		{
			m_PendingTransitionFrom = newName;
		}

		if (m_StatePendingDeletion == oldName)
		{
			m_StatePendingDeletion = newName;
		}

		return true;
	}

	void buki::AnimationStateMachineEditorWindow::DeleteState(
		const std::string& name)
	{
		if (FindState(name) == nullptr)
		{
			return;
		}

		for (AnimationState& state : m_Asset.states)
		{
			auto& transitions = state.transitions;

			transitions.erase(
				std::remove_if(
					transitions.begin(),
					transitions.end(),
					[&](const AnimationTransition& transition)
					{
						return transition.to == name;
					}),
				transitions.end());
		}

		m_Asset.states.erase(
			std::remove_if(
				m_Asset.states.begin(),
				m_Asset.states.end(),
				[&](const AnimationState& state)
				{
					return state.name == name;
				}),
			m_Asset.states.end());

		m_Asset.layout.erase(
			std::remove_if(
				m_Asset.layout.begin(),
				m_Asset.layout.end(),
				[&](const AnimationStateLayout& layout)
				{
					return layout.stateName == name;
				}),
			m_Asset.layout.end());

		if (m_Asset.initialState == name)
		{
			m_Asset.initialState.clear();
		}

		if (m_SelectedStateName == name)
		{
			m_SelectedStateName.clear();
		}

		// Incoming transitions may have been erased, shifting indices.
		m_SelectedTransitionFrom.clear();
		m_SelectedTransitionIndex = -1;

		if (m_PendingTransitionFrom == name)
		{
			m_PendingTransitionFrom.clear();
			m_DraggingTransition = false;
		}
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