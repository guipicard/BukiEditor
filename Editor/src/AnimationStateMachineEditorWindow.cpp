#pragma once
#include "AnimationStateMachineEditorWindow.h"
#include "PropertyInfo.h"
#include "InspectorPropertyDrawer.h"

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


	void AnimationStateMachineEditorWindow::Open(const std::string& path)
	{
		AnimationAsset loaded;

		if (!loaded.LoadFromFile(path))
			return;

		m_AssetPath = path;
		m_Asset = std::move(loaded);
		m_RequestDockNextToScene = true;
		m_Open = true;
		ImGui::SetNextWindowFocus();
		m_RequestFocus = false;

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

		m_RequestDockNextToScene = false;
	}

	bool AnimationStateMachineEditorWindow::IsOpenForPath(std::string path) const
	{
		return m_Open && path == m_AssetPath;
	}


	void AnimationStateMachineEditorWindow::RequestFocus()
	{
		m_RequestFocus = true;
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
			AddStateAtWorldPosition(100.0f, 100.0f);
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
		constexpr float inspectorHeight = 280.0f;

		ImVec2 canvasSize = ImGui::GetContentRegionAvail();
		canvasSize.y -= inspectorHeight + ImGui::GetStyle().ItemSpacing.y;

		if (canvasSize.y < 100.0f)
			canvasSize.y = 100.0f;

		ImGui::BeginChild("StateMachineCanvas", canvasSize, true);

		const bool canvasHovered = ImGui::IsWindowHovered();

		if (canvasHovered &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
		{
			m_PanningCanvas = true;
		}

		if (m_PanningCanvas)
		{
			if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
			{
				const ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;

				m_CanvasOffset.x += mouseDelta.x;
				m_CanvasOffset.y += mouseDelta.y;
			}
			else
			{
				m_PanningCanvas = false;
			}
		}

		if (canvasHovered &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
			!ImGui::IsAnyItemHovered())
		{
			ImGui::OpenPopup("StateMachineCanvasContextMenu");
		}

		if (ImGui::BeginPopup("StateMachineCanvasContextMenu"))
		{
			if (ImGui::MenuItem("Add State Here"))
			{
				const ImVec2 mousePos = ImGui::GetIO().MousePos;

				AddStateAtWorldPosition(
					mousePos.x - m_CanvasOrigin.x - m_CanvasOffset.x,
					mousePos.y - m_CanvasOrigin.y - m_CanvasOffset.y);
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

	void buki::AnimationStateMachineEditorWindow::AddStateAtWorldPosition(
		float x,
		float y)
	{
		AnimationState state;
		state.name = "NewState";
		state.clipPath = "";

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

		PropertyInfo clipPathProperty =
			BUKI_PROP_STRING(AnimationState, clipPath);

		InspectorPropertyDrawer::DrawStringAssetPicker(
			clipPathProperty,
			&state->clipPath,
			"CLIP"
		);

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