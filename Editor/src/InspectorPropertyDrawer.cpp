#pragma once
#include "InspectorPropertyDrawer.h"

#include "ComponentFactory.h"
#include "Engine.h"
#include "IWorld.h"

#include "Entity.h"
#include "MonoBehaviour.h"

#include "EditorAssetEntries.h"
#include "EditorViewportHelpers.h"
#include "PropertyInfo.h"
#include "EntityRef.h"
#include "Button.h"
#include "ScriptFunctionRegistry.h"
#include "TileLayer.h"
#include "BukiContainers.h"

namespace
{
	float GetInspectorValueWidth()
	{
		const float avail = ImGui::GetContentRegionAvail().x;
		return (avail > 60.0f) ? avail : 60.0f;
	}

	float GetInspectorSplitValueWidth(int columns, float spacing = 4.0f)
	{
		const float avail = GetInspectorValueWidth();
		const float totalSpacing = spacing * (columns - 1);
		const float width = (avail - totalSpacing) / static_cast<float>(columns);
		return (width > 24.0f) ? width : 24.0f;
	}
}

namespace
{
	struct MixedTextState
	{
		bool initialized = false;
		std::string value;
	};

	static struct FloatingAssetPickerState
	{
		bool open = false;
		bool setInitialPos = false;
		ImVec2 initialPos{};
		std::string search;
	};

	static std::unordered_map<ImGuiID, FloatingAssetPickerState> sAssetPickerStates;
	static std::unordered_map<ImGuiID, MixedTextState> sMixedTextStates;

	static char sAssetSearchBuffer[256] = {};


	bool AreAllEqualVector2X(const std::vector<buki::Vector2>& values)
	{
		if (values.empty())
			return true;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (values[i].x != values[0].x)
				return false;
		}
		return true;
	}

	bool AreAllEqualVector2Y(const std::vector<buki::Vector2>& values)
	{
		if (values.empty())
			return true;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (values[i].y != values[0].y)
				return false;
		}
		return true;
	}

	template<typename T>
	bool AreAllEqualValues(const std::vector<T>& values)
	{
		if (values.empty())
			return true;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (!(values[i] == values[0]))
				return false;
		}
		return true;
	}

	bool AreAllEqualEntityRefs(const std::vector<buki::EntityRef*>& refs)
	{
		if (refs.empty())
			return true;

		const buki::EntityRef* first = refs[0];
		if (first == nullptr)
			return false;

		for (size_t i = 1; i < refs.size(); ++i)
		{
			const buki::EntityRef* current = refs[i];
			if (current == nullptr)
				return false;

			if (current->entityName != first->entityName)
				return false;
			if (current->prefabPath != first->prefabPath)
				return false;
		}

		return true;
	}

	template<typename T>
	std::vector<T*> GatherSharedFieldPtrs(
		const std::string& componentTypeName,
		const std::vector<buki::Entity*>& entities,
		size_t offset)
	{
		std::vector<T*> result;
		result.reserve(entities.size());

		for (buki::Entity* entity : entities)
		{
			if (entity == nullptr)
				return {};

			buki::Component* component = entity->GetComponentByTypeName(componentTypeName);
			if (component == nullptr)
				return {};

			char* base = reinterpret_cast<char*>(component);
			result.push_back(reinterpret_cast<T*>(base + offset));
		}

		return result;
	}

	enum class RectHandle
	{
		None,
		Move,
		Left, Right, Top, Bottom,
		TopLeft, TopRight, BottomLeft, BottomRight
	};

	struct TileRectSelectorState
	{
		bool open = false;
		bool setInitialPos = false;
		ImVec2 initialPos{};
		float zoom = 1.0f;

		RectHandle activeHandle = RectHandle::None;
		ImVec2 dragStartMouse{};
		buki::RectF dragStartRect;
	};

	static std::unordered_map<ImGuiID, TileRectSelectorState> sTileRectPickerStates;
	static std::unordered_map<ImGuiID, std::vector<size_t>> sTileSelections;

	void ClampRectToImage(buki::RectF& r, float imageW, float imageH)
	{
		if (r.w < 1.0f) r.w = 1.0f;
		if (r.h < 1.0f) r.h = 1.0f;
		if (r.x < 0.0f) r.x = 0.0f;
		if (r.y < 0.0f) r.y = 0.0f;
		if (r.x + r.w > imageW) r.w = imageW - r.x;
		if (r.y + r.h > imageH) r.h = imageH - r.y;
		if (r.w < 1.0f) { r.w = 1.0f; r.x = std::max(0.0f, imageW - 1.0f); }
		if (r.h < 1.0f) { r.h = 1.0f; r.y = std::max(0.0f, imageH - 1.0f); }
	}

	static void ClampRectPositionToImage(buki::RectF& r, float imageW, float imageH)
	{
		if (r.w > imageW) r.w = imageW;
		if (r.h > imageH) r.h = imageH;

		r.x = std::clamp(r.x, 0.0f, imageW - r.w);
		r.y = std::clamp(r.y, 0.0f, imageH - r.h);
	}

	template<typename T>
	void ApplySharedValue(const std::vector<T*>& values, const T& value)
	{
		for (T* ptr : values)
		{
			if (ptr != nullptr)
				*ptr = value;
		}
	}

	MixedTextState& GetMixedTextState(ImGuiID id, const std::string& initialValue)
	{
		MixedTextState& state = sMixedTextStates[id];
		if (!state.initialized)
		{
			state.initialized = true;
			state.value = initialValue;
		}
		return state;
	}

	void ResetMixedTextState(ImGuiID id)
	{
		sMixedTextStates.erase(id);
	}

	void DrawMixedLabel(const std::string& label)
	{
		ImGui::TextUnformatted(label.c_str());
		ImGui::SameLine();
		ImGui::TextDisabled("(mixed)");
	}

	// -------- Draw calls

	bool DrawTileSourceRectWindow(
		const char* title,
		const std::string& imagePath,
		buki::RectF& sourceRectPixels,
		TileRectSelectorState& state)
	{
		if (!state.open)
			return false;

		bool changed = false;

		if (state.setInitialPos)
		{
			ImVec2 windowSize{ 720, 520 };
			ImGui::SetNextWindowPos(
				ImVec2(state.initialPos.x - windowSize.x, state.initialPos.y),
				ImGuiCond_Appearing);
			ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);
			state.setInitialPos = false;
		}

		if (!ImGui::Begin(title, &state.open, ImGuiWindowFlags_NoCollapse))
		{
			ImGui::End();
			return false;
		}

		const buki::Texture2D* texture = buki::TryGetPreviewTexture(imagePath);
		if (texture == nullptr || !texture->IsValid())
		{
			ImGui::TextDisabled("Invalid texture");
			ImGui::End();
			return false;
		}

		const int texW = texture->width;
		const int texH = texture->height;

		ClampRectToImage(sourceRectPixels, (float)texW, (float)texH);

		int src[4] =
		{
			(int)sourceRectPixels.x,
			(int)sourceRectPixels.y,
			(int)sourceRectPixels.w,
			(int)sourceRectPixels.h
		};

		ImGui::Text("Source Rect");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(260.0f);
		if (ImGui::DragInt4("##Source Rect", src, 1, 0, 0))
		{
			sourceRectPixels = { (float)src[0], (float)src[1], (float)src[2], (float)src[3] };
			ClampRectToImage(sourceRectPixels, (float)texW, (float)texH);
			changed = true;
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(120.0f);
		if (ImGui::SliderFloat("Zoom", &state.zoom, 0.1f, 8.0f, "%.2fx"))
			changed = true;

		ImGui::BeginChild("##AtlasCanvasRegion", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollWithMouse);

		const ImVec2 canvasMin = ImGui::GetCursorScreenPos();
		const ImVec2 canvasAvail = ImGui::GetContentRegionAvail();
		ImGui::InvisibleButton("##AtlasCanvas", canvasAvail);

		const ImVec2 imageDrawSize(texW * state.zoom, texH * state.zoom);
		const ImVec2 imageScreenMin(
			canvasMin.x + std::max(0.0f, (canvasAvail.x - imageDrawSize.x) * 0.5f),
			canvasMin.y + std::max(0.0f, (canvasAvail.y - imageDrawSize.y) * 0.5f));
		const ImVec2 imageScreenMax(imageScreenMin.x + imageDrawSize.x, imageScreenMin.y + imageDrawSize.y);

		ImDrawList* dl = ImGui::GetWindowDrawList();
		dl->AddImage(texture->id, imageScreenMin, imageScreenMax);

		const ImVec2 r0(
			imageScreenMin.x + sourceRectPixels.x * state.zoom,
			imageScreenMin.y + sourceRectPixels.y * state.zoom);
		const ImVec2 r1(
			r0.x + sourceRectPixels.w * state.zoom,
			r0.y + sourceRectPixels.h * state.zoom);

		auto PointInRect = [](const ImVec2& p, const ImVec2& a, const ImVec2& b)
			{
				return p.x >= a.x && p.x <= b.x && p.y >= a.y && p.y <= b.y;
			};

		const float handleSize = 6.0f;
		const ImVec2 mouse = ImGui::GetIO().MousePos;

		const ImVec2 tl = r0;
		const ImVec2 tr(r1.x, r0.y);
		const ImVec2 bl(r0.x, r1.y);
		const ImVec2 br = r1;

		auto MakeHandleRect = [&](const ImVec2& c)
			{
				return std::pair<ImVec2, ImVec2>(
					ImVec2(c.x - handleSize, c.y - handleSize),
					ImVec2(c.x + handleSize, c.y + handleSize));
			};

		const auto hTL = MakeHandleRect(tl);
		const auto hTR = MakeHandleRect(tr);
		const auto hBL = MakeHandleRect(bl);
		const auto hBR = MakeHandleRect(br);

		const bool overTL = PointInRect(mouse, hTL.first, hTL.second);
		const bool overTR = PointInRect(mouse, hTR.first, hTR.second);
		const bool overBL = PointInRect(mouse, hBL.first, hBL.second);
		const bool overBR = PointInRect(mouse, hBR.first, hBR.second);

		RectHandle hoveredHandle = RectHandle::None;

		if (overTL) hoveredHandle = RectHandle::TopLeft;
		else if (overTR) hoveredHandle = RectHandle::TopRight;
		else if (overBL) hoveredHandle = RectHandle::BottomLeft;
		else if (overBR) hoveredHandle = RectHandle::BottomRight;
		else if (std::abs(mouse.x - r0.x) <= handleSize && mouse.y >= r0.y && mouse.y <= r1.y) hoveredHandle = RectHandle::Left;
		else if (std::abs(mouse.x - r1.x) <= handleSize && mouse.y >= r0.y && mouse.y <= r1.y) hoveredHandle = RectHandle::Right;
		else if (std::abs(mouse.y - r0.y) <= handleSize && mouse.x >= r0.x && mouse.x <= r1.x) hoveredHandle = RectHandle::Top;
		else if (std::abs(mouse.y - r1.y) <= handleSize && mouse.x >= r0.x && mouse.x <= r1.x) hoveredHandle = RectHandle::Bottom;
		else if (PointInRect(mouse, r0, r1)) hoveredHandle = RectHandle::Move;

		const bool mouseOverImage =
			mouse.x >= imageScreenMin.x && mouse.x <= imageScreenMax.x &&
			mouse.y >= imageScreenMin.y && mouse.y <= imageScreenMax.y;

		const bool mouseOverAnyHandle = overTL || overTR || overBL || overBR;

		if ((mouseOverImage || mouseOverAnyHandle) &&
			hoveredHandle != RectHandle::None &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			state.activeHandle = hoveredHandle;
			state.dragStartMouse = mouse;
			state.dragStartRect = sourceRectPixels;
		}

		if (state.activeHandle != RectHandle::None && ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			const ImVec2 delta(
				(mouse.x - state.dragStartMouse.x) / state.zoom,
				(mouse.y - state.dragStartMouse.y) / state.zoom);

			buki::RectF next = state.dragStartRect;

			switch (state.activeHandle)
			{
			case RectHandle::Move:
				next.x += delta.x;
				next.y += delta.y;
				ClampRectPositionToImage(next, (float)texW, (float)texH);
				break;

			case RectHandle::Left:
				next.x += delta.x;
				next.w -= delta.x;
				break;

			case RectHandle::Right:
				next.w += delta.x;
				break;

			case RectHandle::Top:
				next.y += delta.y;
				next.h -= delta.y;
				break;

			case RectHandle::Bottom:
				next.h += delta.y;
				break;

			case RectHandle::TopLeft:
				next.x += delta.x;
				next.w -= delta.x;
				next.y += delta.y;
				next.h -= delta.y;
				break;

			case RectHandle::TopRight:
				next.w += delta.x;
				next.y += delta.y;
				next.h -= delta.y;
				break;

			case RectHandle::BottomLeft:
				next.x += delta.x;
				next.w -= delta.x;
				next.h += delta.y;
				break;

			case RectHandle::BottomRight:
				next.w += delta.x;
				next.h += delta.y;
				break;

			default:
				break;
			}

			if (state.activeHandle != RectHandle::Move)
			{
				const float fixedRight = state.dragStartRect.x + state.dragStartRect.w;
				const float fixedBottom = state.dragStartRect.y + state.dragStartRect.h;

				if (state.activeHandle == RectHandle::Left ||
					state.activeHandle == RectHandle::TopLeft ||
					state.activeHandle == RectHandle::BottomLeft)
				{
					if (next.x < 0.0f)
					{
						next.x = 0.0f;
						next.w = fixedRight - next.x;
					}
				}

				if (state.activeHandle == RectHandle::Top ||
					state.activeHandle == RectHandle::TopLeft ||
					state.activeHandle == RectHandle::TopRight)
				{
					if (next.y < 0.0f)
					{
						next.y = 0.0f;
						next.h = fixedBottom - next.y;
					}
				}

				if (state.activeHandle == RectHandle::Right ||
					state.activeHandle == RectHandle::TopRight ||
					state.activeHandle == RectHandle::BottomRight)
				{
					if (next.x + next.w > (float)texW)
						next.w = (float)texW - next.x;
				}

				if (state.activeHandle == RectHandle::Bottom ||
					state.activeHandle == RectHandle::BottomLeft ||
					state.activeHandle == RectHandle::BottomRight)
				{
					if (next.y + next.h > (float)texH)
						next.h = (float)texH - next.y;
				}

				if (next.w < 1.0f)
				{
					next.w = 1.0f;
					if (state.activeHandle == RectHandle::Left ||
						state.activeHandle == RectHandle::TopLeft ||
						state.activeHandle == RectHandle::BottomLeft)
					{
						next.x = state.dragStartRect.x + state.dragStartRect.w - 1.0f;
					}
				}

				if (next.h < 1.0f)
				{
					next.h = 1.0f;
					if (state.activeHandle == RectHandle::Top ||
						state.activeHandle == RectHandle::TopLeft ||
						state.activeHandle == RectHandle::TopRight)
					{
						next.y = state.dragStartRect.y + state.dragStartRect.h - 1.0f;
					}
				}

				ClampRectToImage(next, (float)texW, (float)texH);
			}

			if (next.x != sourceRectPixels.x || next.y != sourceRectPixels.y ||
				next.w != sourceRectPixels.w || next.h != sourceRectPixels.h)
			{
				sourceRectPixels = next;
				changed = true;
			}
		}

		if (state.activeHandle != RectHandle::None && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			state.activeHandle = RectHandle::None;

		dl->AddRectFilled(r0, r1, IM_COL32(80, 160, 255, 40));
		dl->AddRect(r0, r1, IM_COL32(80, 160, 255, 255), 0.0f, 0, 2.0f);

		auto DrawHandle = [&](ImVec2 c)
			{
				dl->AddRectFilled(
					ImVec2(c.x - handleSize, c.y - handleSize),
					ImVec2(c.x + handleSize, c.y + handleSize),
					IM_COL32(255, 255, 255, 230));
				dl->AddRect(
					ImVec2(c.x - handleSize, c.y - handleSize),
					ImVec2(c.x + handleSize, c.y + handleSize),
					IM_COL32(20, 20, 20, 255));
			};

		DrawHandle(tl);
		DrawHandle(tr);
		DrawHandle(bl);
		DrawHandle(br);

		ImGui::EndChild();
		ImGui::End();

		return changed;
	}

	bool DrawInlineAssetPreview(
		std::string& path,
		const char* payloadType,
		float thumbnailSize = 48.0f,
		buki::RectF sourceRect = buki::RectF{ 0.0f, 0.0f, 0.0f, 0.0f })
	{
		const buki::Texture2D* texture = nullptr;

		if (!path.empty())
		{
			if (std::strcmp(payloadType, "IMAGE") == 0 ||
				std::strcmp(payloadType, "AUDIO") == 0 ||
				std::strcmp(payloadType, "PREFAB") == 0)
			{
				texture = buki::GetBrowserThumbnail(path, false);
			}
		}

		if (texture == nullptr || !texture->IsValid())
			texture = buki::GetBrowserThumbnail("", false);

		ImGui::PushID(("Preview_" + path + payloadType).c_str());

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.08f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.14f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1, 1, 1, 0.10f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

		bool clicked = false;
		const ImVec2 buttonSize(thumbnailSize, thumbnailSize);

		if (texture != nullptr && texture->IsValid())
		{
			const ImVec2 fullImageSize((float)texture->width, (float)texture->height);

			ImVec2 imageSize(thumbnailSize, thumbnailSize);
			ImVec2 padding(8.0f, 8.0f);

			if (fullImageSize.x > 0.0f && fullImageSize.y > 0.0f && fullImageSize.x != fullImageSize.y)
			{
				const float aspect = fullImageSize.x / fullImageSize.y;
				if (aspect > 1.0f)
				{
					imageSize.y /= aspect;
					padding.y += (thumbnailSize - imageSize.y) * 0.5f;
				}
				else
				{
					imageSize.x *= aspect;
					padding.x += (thumbnailSize - imageSize.x) * 0.5f;
				}
			}

			const bool hasSourceRect = sourceRect.w > 0.0f && sourceRect.h > 0.0f;

			ImVec2 uv0(0.0f, 0.0f);
			ImVec2 uv1(1.0f, 1.0f);

			if (hasSourceRect)
			{
				buki::RectF previewRect = sourceRect;
				previewRect.x = std::clamp(previewRect.x, 0.0f, fullImageSize.x);
				previewRect.y = std::clamp(previewRect.y, 0.0f, fullImageSize.y);
				previewRect.w = std::clamp(previewRect.w, 0.0f, fullImageSize.x - previewRect.x);
				previewRect.h = std::clamp(previewRect.h, 0.0f, fullImageSize.y - previewRect.y);

				uv0.x = previewRect.x / fullImageSize.x;
				uv0.y = previewRect.y / fullImageSize.y;
				uv1.x = (previewRect.x + previewRect.w) / fullImageSize.x;
				uv1.y = (previewRect.y + previewRect.h) / fullImageSize.y;
			}

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);

			clicked = ImGui::ImageButton(
				"##inline_asset_preview",
				static_cast<ImTextureID>(texture->id),
				imageSize,
				uv0,
				uv1,
				ImVec4(0, 0, 0, 0),
				ImVec4(1, 1, 1, 1));

			ImGui::PopStyleVar();

			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::TextUnformatted(path.empty() ? "None" : buki::DisplayNameForPath(path).c_str());
			ImGui::TextDisabled("%s", path.empty() ? "No asset selected" : fs::path(path).parent_path().generic_string().c_str());
			ImGui::EndGroup();
		}
		else
		{
			clicked = ImGui::Button("##inline_asset_preview_empty", buttonSize);
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadType))
			{
				const char* dropped = static_cast<const char*>(payload->Data);
				if (dropped != nullptr)
					path = dropped;
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor(4);
		ImGui::PopID();

		return clicked;
	}

	bool DrawAssetPickerWindow(
		const char* title,
		const char* payloadType,
		std::string& path,
		FloatingAssetPickerState& state)
	{
		if (!state.open)
			return false;

		bool changed = false;

		if (state.setInitialPos)
		{
			ImVec2 windowSize{ 720, 430 };
			ImGui::SetNextWindowPos(
				ImVec2(state.initialPos.x - windowSize.x, state.initialPos.y),
				ImGuiCond_Appearing);
			ImGui::SetNextWindowSize(windowSize, ImGuiCond_Appearing);
			state.setInitialPos = false;
		}

		if (!ImGui::Begin(title, &state.open,
			ImGuiWindowFlags_NoCollapse))
		{
			ImGui::End();
			return false;
		}

		char searchBuffer[256] = {};
		strncpy_s(searchBuffer, state.search.c_str(), sizeof(searchBuffer) - 1);
		if (ImGui::InputTextWithHint("##AssetSearch", "Search assets...", searchBuffer, sizeof(searchBuffer)))
			state.search = searchBuffer;

		ImGui::SameLine();
		if (ImGui::Button("Clear Search"))
			state.search.clear();

		ImGui::Separator();

		if (ImGui::Button("None"))
		{
			path.clear();
			changed = true;
		}

		static std::unordered_map<std::string, std::vector<buki::BrowserEntry>> sCachedEntries;
		const std::string cacheKey = std::string(payloadType);
		if (sCachedEntries.find(cacheKey) == sCachedEntries.end())
		{
			sCachedEntries[cacheKey] = buki::CollectBrowserEntries("../Deployment", payloadType);
		}
		ImGui::SameLine();
		if (ImGui::Button("Refresh"))
		{
			sCachedEntries[cacheKey] = buki::CollectBrowserEntries("../Deployment", payloadType);
		}

		const std::vector<buki::BrowserEntry>& allEntries = sCachedEntries[cacheKey];
		const std::string filter = buki::ToLowerCopy(searchBuffer);

		const float thumbnailSize = 100.0f;
		const float padding = thumbnailSize / 4.0f;
		const float pickerWidth = ImGui::GetWindowWidth() - 14.0f;
		//const float pickerWidth = 720.0f;
		const float pickerHeight = ImGui::GetWindowHeight();
		//const float pickerHeight = 430.0f;
		const float cellSize = thumbnailSize + padding;

		int columnCount = static_cast<int>((pickerWidth / cellSize));
		if (columnCount < 1)
			columnCount = 1;

		ImGui::BeginChild("##AssetPickerGrid", ImVec2(pickerWidth, 0), true);
		ImGui::Columns(columnCount, nullptr, false);

		for (const buki::BrowserEntry& item : allEntries)
		{
			const std::string relativePath = buki::ToLowerCopy(item.path);

			const std::string file = buki::ToLowerCopy(item.displayName);

			if (!filter.empty() &&
				relativePath.find(filter) == std::string::npos &&
				file.find(filter) == std::string::npos)
			{
				continue;
			}

			const bool selected = (!path.empty() && fs::path(path).lexically_normal() == relativePath);
			const bool clicked = buki::DrawBrowserTile(item, thumbnailSize, selected);
			const bool doubleClicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);

			if (clicked)
			{
				path = relativePath;
				changed = true;
			}

			if (doubleClicked)
			{
				path = relativePath;
				changed = true;
				state.open = false;
			}

			ImGui::NextColumn();
		}

		ImGui::Columns(1);
		ImGui::EndChild();

		ImGui::End();
		return changed;
	}

	bool DrawAssetListEditor(
		const char* label,
		std::vector<std::string>& values,
		const char* payloadType,
		bool mixed,
		bool& applyRequested,
		bool& clearRequested)
	{
		bool changed = false;
		applyRequested = false;
		clearRequested = false;

		if (mixed)
		{
			ImGui::TextDisabled("(mixed)");
			ImGui::TextDisabled("Editing this list will replace the values on all selected entities.");
		}

		int removeIndex = -1;
		for (int i = 0; i < static_cast<int>(values.size()); ++i)
		{
			ImGui::PushID(i);

			std::string displayName = buki::DisplayNameForPath(values[i]);
			if (displayName.empty())
				displayName = "None";

			const ImGuiID pickerId = ImGui::GetID("AssetPickerWindow");
			FloatingAssetPickerState& state = sAssetPickerStates[pickerId];

			if (DrawInlineAssetPreview(values[i], payloadType, 44.0f))
			{
				state.open = true;
				state.setInitialPos = true;
				state.initialPos = ImGui::GetItemRectMin();
			}

			changed |= DrawAssetPickerWindow("Asset Picker##AssetPickerWindow", payloadType, values[i], state);


			if (ImGui::Button("-"))
				removeIndex = i;



			ImGui::PopID();

		}

		if (removeIndex >= 0)
		{
			values.erase(values.begin() + removeIndex);
			changed = true;
		}

		if (ImGui::Button(("Add##" + std::string(label)).c_str()))
		{
			values.push_back("");
			changed = true;
		}

		if (mixed)
		{
			if (changed)
				applyRequested = true;
			clearRequested = true;
		}

		return changed;
	}

	// ------ Mixed Fields
	
	bool DrawMixedBoolField(const char* id, bool mixed, bool& value)
	{
		ImGui::PushID(id);
		ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixed);
		const bool changed = ImGui::Checkbox("##Value", &value);
		ImGui::PopItemFlag();
		ImGui::PopID();
		return changed;
	}

	static bool DrawMixedRectFDragFieldPerAxis(
		const char* id,
		bool mixedX,
		bool mixedY,
		bool mixedW,
		bool mixedH,
		float& x,
		float& y,
		float& w,
		float& h,
		float& deltaX,
		float& deltaY,
		float& deltaW,
		float& deltaH,
		bool& changedX,
		bool& changedY,
		bool& changedW,
		bool& changedH,
		float speed = 0.1f,
		const char* format = "%.1f")
	{
		changedX = false;
		changedY = false;
		changedW = false;
		changedH = false;

		deltaX = 0.0f;
		deltaY = 0.0f;
		deltaW = 0.0f;
		deltaH = 0.0f;

		const float originalX = x;
		const float originalY = y;
		const float originalW = w;
		const float originalH = h;

		ImGui::PushID(id);

		const float avail = ImGui::GetContentRegionAvail().x;
		const float axisWidth = 14.0f;
		const float spacing = ImGui::GetStyle().ItemSpacing.x;
		const bool compact = avail < 220.0f;

		if (compact)
		{
			const float fieldWidth = std::max(20.0f, (avail - (axisWidth * 2.0f) - (spacing * 3.0f)) * 0.5f);

			ImGui::TextUnformatted("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedX);
			if (ImGui::DragFloat("##X", &x, speed, 0.0f, 0.0f, format))
			{
				deltaX = x - originalX;
				changedX = true;
			}
			ImGui::PopItemFlag();

			ImGui::SameLine();
			ImGui::TextUnformatted("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedY);
			if (ImGui::DragFloat("##Y", &y, speed, 0.0f, 0.0f, format))
			{
				deltaY = y - originalY;
				changedY = true;
			}
			ImGui::PopItemFlag();

			ImGui::TextUnformatted("W");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedW);
			if (ImGui::DragFloat("##W", &w, speed, 0.0f, 0.0f, format))
			{
				deltaW = w - originalW;
				changedW = true;
			}
			ImGui::PopItemFlag();

			ImGui::SameLine();
			ImGui::TextUnformatted("H");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedH);
			if (ImGui::DragFloat("##H", &h, speed, 0.0f, 0.0f, format))
			{
				deltaH = h - originalH;
				changedH = true;
			}
			ImGui::PopItemFlag();
		}
		else
		{
			const float fieldWidth = std::max(18.0f, (avail - (axisWidth * 4.0f) - (spacing * 7.0f)) * 0.25f);

			ImGui::TextUnformatted("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedX);
			if (ImGui::DragFloat("##X", &x, speed, 0.0f, 0.0f, format))
			{
				deltaX = x - originalX;
				changedX = true;
			}
			ImGui::PopItemFlag();

			ImGui::SameLine();
			ImGui::TextUnformatted("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedY);
			if (ImGui::DragFloat("##Y", &y, speed, 0.0f, 0.0f, format))
			{
				deltaY = y - originalY;
				changedY = true;
			}
			ImGui::PopItemFlag();

			ImGui::SameLine();
			ImGui::TextUnformatted("W");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedW);
			if (ImGui::DragFloat("##W", &w, speed, 0.0f, 0.0f, format))
			{
				deltaW = w - originalW;
				changedW = true;
			}
			ImGui::PopItemFlag();

			ImGui::SameLine();
			ImGui::TextUnformatted("H");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedH);
			if (ImGui::DragFloat("##H", &h, speed, 0.0f, 0.0f, format))
			{
				deltaH = h - originalH;
				changedH = true;
			}
			ImGui::PopItemFlag();
		}

		ImGui::PopID();
		return changedX || changedY || changedW || changedH;
	}

	static bool DrawMixedVector2DragDeltaFieldPerAxis(
		const char* id,
		bool mixedX,
		bool mixedY,
		float& x,
		float& y,
		float& deltaX,
		float& deltaY,
		bool& changedX,
		bool& changedY,
		const char* format = "%.1f")
	{
		changedX = false;
		changedY = false;
		deltaX = 0.0f;
		deltaY = 0.0f;

		const float originalX = x;
		const float originalY = y;

		ImGui::PushID(id);

		const float totalWidth = ImGui::GetContentRegionAvail().x;
		const float axisWidth = 14.0f;
		const float spacing = ImGui::GetStyle().ItemSpacing.x;
		const float fieldWidth = std::max(20.0f, (totalWidth - (axisWidth * 2.0f) - (spacing * 3.0f)) * 0.5f);

		ImGui::TextUnformatted("X");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(fieldWidth);
		ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedX);
		if (ImGui::DragFloat("##X", &x, 0.1f, 0.0f, 0.0f, format))
		{
			deltaX = x - originalX;
			changedX = true;
		}
		ImGui::PopItemFlag();

		ImGui::SameLine();

		ImGui::TextUnformatted("Y");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(fieldWidth);
		ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedY);
		if (ImGui::DragFloat("##Y", &y, 0.1f, 0.0f, 0.0f, format))
		{
			deltaY = y - originalY;
			changedY = true;
		}
		ImGui::PopItemFlag();

		ImGui::PopID();
		return changedX || changedY;
	}

	bool DrawMixedStringEditor(const char* id, bool mixed, std::string& value, float width = 0.0f)
	{
		bool changed = false;

		ImGui::PushID(id);
		const ImGuiID fieldId = ImGui::GetID("MixedStringField");

		const std::string initialDisplay = mixed ? "-" : value;
		MixedTextState& state = GetMixedTextState(fieldId, initialDisplay);

		char buffer[512] = {};
		strncpy_s(buffer, state.value.c_str(), sizeof(buffer) - 1);

		const float resolvedWidth = width > 0.0f ? width : ImGui::GetContentRegionAvail().x;
		ImGui::SetNextItemWidth(resolvedWidth);

		if (ImGui::InputText("##MixedStringField", buffer, sizeof(buffer)))
		{
			state.value = buffer;
			if (state.value != "-")
			{
				value = state.value;
				changed = true;
			}
		}

		if (!ImGui::IsItemActive())
			ResetMixedTextState(fieldId);

		ImGui::PopID();
		return changed;
	}

	// ------ Core helpers

	static bool DrawMixedIntDragDeltaField(
		const char* id,
		bool mixed,
		int& value,
		int& delta,
		float speed = 1.0f)
	{
		delta = 0;
		const int originalValue = value;

		ImGui::PushID(id);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixed);
		const bool changed = ImGui::DragInt("##Value", &value, speed);
		ImGui::PopItemFlag();

		ImGui::PopID();

		if (!changed)
			return false;

		delta = value - originalValue;
		return true;
	}

	static bool DrawMixedFloatDragDeltaField(
		const char* id,
		bool mixed,
		float& value,
		float& delta,
		float speed = 0.1f,
		const char* format = "%.1f")
	{
		delta = 0.0f;
		const float originalValue = value;

		ImGui::PushID(id);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixed);
		const bool changed = ImGui::DragFloat("##Value", &value, speed, 0.0f, 0.0f, format);
		ImGui::PopItemFlag();

		ImGui::PopID();

		if (!changed)
			return false;

		delta = value - originalValue;
		return true;
	}

	static bool DrawMixedRectFDragDeltaFieldPerAxis(
		const char* id,
		bool mixedX,
		bool mixedY,
		bool mixedW,
		bool mixedH,
		float& x,
		float& y,
		float& w,
		float& h,
		float& deltaX,
		float& deltaY,
		float& deltaW,
		float& deltaH,
		bool& changedX,
		bool& changedY,
		bool& changedW,
		bool& changedH,
		float speed = 0.1f,
		const char* format = "%.1f")
	{
		changedX = false;
		changedY = false;
		changedW = false;
		changedH = false;

		deltaX = 0.0f;
		deltaY = 0.0f;
		deltaW = 0.0f;
		deltaH = 0.0f;

		const float originalX = x;
		const float originalY = y;
		const float originalW = w;
		const float originalH = h;

		ImGui::PushID(id);

		const float avail = ImGui::GetContentRegionAvail().x;
		const float axisWidth = 14.0f;
		const float spacing = ImGui::GetStyle().ItemSpacing.x;
		const bool compact = avail < 220.0f;

		if (compact)
		{
			const float fieldWidth = std::max(20.0f, (avail - (axisWidth * 2.0f) - (spacing * 3.0f)) * 0.5f);

			ImGui::TextUnformatted("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedX);
			if (ImGui::DragFloat("##X", &x, speed, 0.0f, 0.0f, format))
			{
				deltaX = x - originalX;
				changedX = true;
			}
			ImGui::PopItemFlag();

			ImGui::SameLine();
			ImGui::TextUnformatted("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedY);
			if (ImGui::DragFloat("##Y", &y, speed, 0.0f, 0.0f, format))
			{
				deltaY = y - originalY;
				changedY = true;
			}
			ImGui::PopItemFlag();

			ImGui::TextUnformatted("W");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedW);
			if (ImGui::DragFloat("##W", &w, speed, 0.0f, 0.0f, format))
			{
				deltaW = w - originalW;
				changedW = true;
			}
			ImGui::PopItemFlag();

			ImGui::SameLine();
			ImGui::TextUnformatted("H");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedH);
			if (ImGui::DragFloat("##H", &h, speed, 0.0f, 0.0f, format))
			{
				deltaH = h - originalH;
				changedH = true;
			}
			ImGui::PopItemFlag();
		}
		else
		{
			const float fieldWidth = std::max(18.0f, (avail - (axisWidth * 4.0f) - (spacing * 7.0f)) * 0.25f);

			ImGui::TextUnformatted("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedX);
			if (ImGui::DragFloat("##X", &x, speed, 0.0f, 0.0f, format))
			{
				deltaX = x - originalX;
				changedX = true;
			}
			ImGui::PopItemFlag();

			ImGui::SameLine();
			ImGui::TextUnformatted("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedY);
			if (ImGui::DragFloat("##Y", &y, speed, 0.0f, 0.0f, format))
			{
				deltaY = y - originalY;
				changedY = true;
			}
			ImGui::PopItemFlag();

			ImGui::SameLine();
			ImGui::TextUnformatted("W");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedW);
			if (ImGui::DragFloat("##W", &w, speed, 0.0f, 0.0f, format))
			{
				deltaW = w - originalW;
				changedW = true;
			}
			ImGui::PopItemFlag();

			ImGui::SameLine();
			ImGui::TextUnformatted("H");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedH);
			if (ImGui::DragFloat("##H", &h, speed, 0.0f, 0.0f, format))
			{
				deltaH = h - originalH;
				changedH = true;
			}
			ImGui::PopItemFlag();
		}

		ImGui::PopID();
		return changedX || changedY || changedW || changedH;
	}

	static bool DrawMixedColorDragDeltaField(
		const char* label,
		bool mixedR,
		bool mixedG,
		bool mixedB,
		bool mixedA,
		float& r,
		float& g,
		float& b,
		float& a,
		float& deltaR,
		float& deltaG,
		float& deltaB,
		float& deltaA,
		bool& changedR,
		bool& changedG,
		bool& changedB,
		bool& changedA)
	{
		changedR = false;
		changedG = false;
		changedB = false;
		changedA = false;

		deltaR = 0.0f;
		deltaG = 0.0f;
		deltaB = 0.0f;
		deltaA = 0.0f;

		const float original[4] = { r, g, b, a };
		float rgba[4] = { r, g, b, a };

		ImGui::PushID(label);

		ImGui::TextUnformatted(label);
		ImGui::SameLine();

		ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixedR || mixedG || mixedB || mixedA);
		const bool edited = ImGui::ColorEdit4("##Value", rgba);
		ImGui::PopItemFlag();

		ImGui::PopID();

		if (!edited)
			return false;

		if (rgba[0] != original[0])
		{
			deltaR = rgba[0] - original[0];
			r = rgba[0];
			changedR = true;
		}
		if (rgba[1] != original[1])
		{
			deltaG = rgba[1] - original[1];
			g = rgba[1];
			changedG = true;
		}
		if (rgba[2] != original[2])
		{
			deltaB = rgba[2] - original[2];
			b = rgba[2];
			changedB = true;
		}
		if (rgba[3] != original[3])
		{
			deltaA = rgba[3] - original[3];
			a = rgba[3];
			changedA = true;
		}

		return changedR || changedG || changedB || changedA;
	}

	// ------ Shared Fields

	bool DrawSharedIntProperty(const buki::PropertyInfo& propInfo, const std::vector<int*>& values)
	{
		if (values.empty())
			return false;

		std::vector<buki::BukiField> fields = propInfo.prop.fields;
		buki::BukiField lastField = fields[fields.size() - 1];

		std::vector<int> snapshot;
		snapshot.reserve(values.size());
		for (int* ptr : values)
			snapshot.push_back(ptr ? *ptr : 0);

		const bool mixed = !AreAllEqualValues(snapshot);
		int value = snapshot[0];
		int delta = 0;
		bool edited = false;

		if (propInfo.hasMin && propInfo.hasMax)
		{
			ImGui::PushID(lastField.key.c_str());

			ImGui::TextUnformatted(lastField.name.c_str());
			ImGui::SameLine();

			ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixed);
			edited = ImGui::SliderInt(("##" + lastField.key).c_str(), &value,
				static_cast<int>(propInfo.minValue),
				static_cast<int>(propInfo.maxValue));
			ImGui::PopItemFlag();

			ImGui::PopID();

			if (edited)
				delta = value - snapshot[0];
		}
		else
		{
			ImGui::PushID(lastField.key.c_str());
			ImGui::TextUnformatted(lastField.key.c_str());
			ImGui::SameLine();
			ImGui::PopID();
			edited = DrawMixedIntDragDeltaField(
				lastField.name.c_str(),
				mixed,
				value,
				delta,
				propInfo.dragSpeed > 0.0f ? propInfo.dragSpeed : 1.0f);
		}

		if (!edited)
			return false;

		if (propInfo.hasMin && value < static_cast<int>(propInfo.minValue))
			value = static_cast<int>(propInfo.minValue);
		if (propInfo.hasMax && value > static_cast<int>(propInfo.maxValue))
			value = static_cast<int>(propInfo.maxValue);

		for (int* ptr : values)
		{
			if (ptr == nullptr)
				continue;

			if (mixed && !propInfo.hasMin && !propInfo.hasMax)
			{
				*ptr += delta;

				if (propInfo.hasMin && *ptr < static_cast<int>(propInfo.minValue))
					*ptr = static_cast<int>(propInfo.minValue);
				if (propInfo.hasMax && *ptr > static_cast<int>(propInfo.maxValue))
					*ptr = static_cast<int>(propInfo.maxValue);
			}
			else
			{
				*ptr = value;
			}
		}

		return true;
	}

	bool DrawSharedFloatProperty(const buki::PropertyInfo& propInfo, const std::vector<float*>& values)
	{
		if (values.empty())
			return false;

		std::vector<buki::BukiField> fields = propInfo.prop.fields;
		buki::BukiField lastField = fields[fields.size() - 1];

		std::vector<float> snapshot;
		snapshot.reserve(values.size());
		for (float* ptr : values)
			snapshot.push_back(ptr ? *ptr : 0.0f);

		const bool mixed = !AreAllEqualValues(snapshot);
		float value = snapshot[0];
		float delta = 0.0f;

		ImGui::PushID(lastField.key.c_str());
		ImGui::TextUnformatted(lastField.key.c_str());
		ImGui::SameLine();
		ImGui::PopID();

		if (!DrawMixedFloatDragDeltaField(
			lastField.name.c_str(),
			mixed,
			value,
			delta,
			propInfo.dragSpeed > 0.0f ? propInfo.dragSpeed : 0.1f,
			"%.1f"))
		{
			return false;
		}

		for (float* ptr : values)
		{
			if (ptr == nullptr)
				continue;

			if (mixed) *ptr += delta;
			else *ptr = value;

			if (propInfo.hasMin && *ptr < propInfo.minValue)
				*ptr = propInfo.minValue;
			if (propInfo.hasMax && *ptr > propInfo.maxValue)
				*ptr = propInfo.maxValue;
		}

		return true;
	}

	bool DrawSharedBoolProperty(const buki::PropertyInfo& propInfo, const std::vector<bool*>& values)
	{
		if (values.empty())
			return false;

		std::vector<buki::BukiField> fields = propInfo.prop.fields;
		if (fields.empty())
			return false;

		const buki::BukiField& lastField = fields.back();

		bool value = *values[0];
		bool mixed = false;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (*values[i] != value)
			{
				mixed = true;
				break;
			}
		}

		ImGui::PushID(lastField.name.c_str());

		ImGui::TextUnformatted(lastField.key.c_str());
		ImGui::SameLine();

		ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, mixed);
		const bool edited = ImGui::Checkbox("##Value", &value);
		ImGui::PopItemFlag();

		ImGui::PopID();

		if (!edited)
			return false;

		ApplySharedValue(values, value);
		return true;
	}

	bool DrawSharedStringProperty(const buki::PropertyInfo& propInfo, const std::vector<std::string*>& values)
	{
		if (values.empty())
			return false;

		std::vector<buki::BukiField> fields = propInfo.prop.fields;
		buki::BukiField lastField = fields[fields.size() - 1];

		std::vector<std::string> snapshot;
		snapshot.reserve(values.size());
		for (std::string* ptr : values)
			snapshot.push_back(ptr ? *ptr : "");

		const bool mixed = !AreAllEqualValues(snapshot);
		std::string value = snapshot[0];

		if (!DrawMixedStringEditor(lastField.key.c_str(), mixed, value))
			return false;

		ApplySharedValue(values, value);
		return true;
	}

	bool DrawSharedStringListProperty(const buki::PropertyInfo& propInfo, const std::vector<std::vector<std::string>*>& values)
	{
		if (values.empty())
			return false;

		std::vector<buki::BukiField> fields = propInfo.prop.fields;
		buki::BukiField lastField = fields[fields.size() - 1];

		std::vector<std::string> first = values[0] ? *values[0] : std::vector<std::string>{};
		bool mixed = false;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (values[i] == nullptr || *values[i] != first)
			{
				mixed = true;
				break;
			}
		}

		bool changed = false;
		std::vector<std::string> editValues = first;

		ImGui::Text("%s", lastField.key.c_str());
		ImGui::Indent();

		if (mixed)
		{
			ImGui::TextDisabled("(mixed)");
			ImGui::TextDisabled("Editing will replace values on all selected entities.");
		}

		int removeIndex = -1;
		for (int i = 0; i < static_cast<int>(editValues.size()); ++i)
		{
			ImGui::PushID(i);

			char buffer[256] = {};
			strncpy_s(buffer, editValues[i].c_str(), sizeof(buffer) - 1);

			ImGui::SetNextItemWidth(220.0f);
			if (ImGui::InputText("##value", buffer, sizeof(buffer)))
			{
				editValues[i] = buffer;
				changed = true;
			}

			ImGui::SameLine();
			if (ImGui::Button("-"))
				removeIndex = i;

			ImGui::PopID();
		}

		if (removeIndex >= 0)
		{
			editValues.erase(editValues.begin() + removeIndex);
			changed = true;
		}

		if (ImGui::Button(("Add##" + lastField.key).c_str()))
		{
			editValues.push_back("");
			changed = true;
		}

		ImGui::Unindent();

		if (!changed)
			return false;

		for (std::vector<std::string>* ptr : values)
		{
			if (ptr != nullptr)
				*ptr = editValues;
		}

		return true;
	}

	bool DrawSharedVector2Property(const buki::PropertyInfo& propInfo, const std::vector<buki::Vector2*>& values)
	{
		if (values.empty())
			return false;

		std::vector<buki::BukiField> fields = propInfo.prop.fields;
		if (fields.empty())
			return false;

		const buki::BukiField& lastField = fields.back();

		std::vector<buki::Vector2> snapshot;
		snapshot.reserve(values.size());
		for (buki::Vector2* ptr : values)
			snapshot.push_back(ptr ? *ptr : buki::Vector2{});

		const bool mixedX = !AreAllEqualVector2X(snapshot);
		const bool mixedY = !AreAllEqualVector2Y(snapshot);

		float x = snapshot[0].x;
		float y = snapshot[0].y;
		float deltaX = 0.0f;
		float deltaY = 0.0f;
		bool changedX = false;
		bool changedY = false;

		ImGui::TextUnformatted(lastField.key.c_str());
		ImGui::SameLine();

		if (!DrawMixedVector2DragDeltaFieldPerAxis(
			("##" + lastField.name).c_str(),
			mixedX,
			mixedY,
			x,
			y,
			deltaX,
			deltaY,
			changedX,
			changedY,
			"%.1f"))
		{
			return false;
		}

		for (buki::Vector2* ptr : values)
		{
			if (ptr == nullptr)
				continue;

			if (changedX)
			{
				if (mixedX) ptr->x += deltaX;
				else ptr->x = x;
			}

			if (changedY)
			{
				if (mixedY) ptr->y += deltaY;
				else ptr->y = y;
			}

			if (propInfo.hasMin)
			{
				if (ptr->x < propInfo.minValue) ptr->x = propInfo.minValue;
				if (ptr->y < propInfo.minValue) ptr->y = propInfo.minValue;
			}

			if (propInfo.hasMax)
			{
				if (ptr->x > propInfo.maxValue) ptr->x = propInfo.maxValue;
				if (ptr->y > propInfo.maxValue) ptr->y = propInfo.maxValue;
			}
		}

		return true;
	}

	bool DrawSharedRectFProperty(const buki::PropertyInfo& propInfo, const std::vector<buki::RectF*>& values)
	{
		if (values.empty())
			return false;

		std::vector<buki::BukiField> fields = propInfo.prop.fields;
		if (fields.empty())
			return false;

		const buki::BukiField& lastField = fields.back();

		std::vector<float> xs, ys, ws, hs;
		xs.reserve(values.size());
		ys.reserve(values.size());
		ws.reserve(values.size());
		hs.reserve(values.size());

		for (buki::RectF* ptr : values)
		{
			const buki::RectF rect = ptr ? *ptr : buki::RectF{};
			xs.push_back(rect.x);
			ys.push_back(rect.y);
			ws.push_back(rect.w);
			hs.push_back(rect.h);
		}

		const bool mixedX = !AreAllEqualValues(xs);
		const bool mixedY = !AreAllEqualValues(ys);
		const bool mixedW = !AreAllEqualValues(ws);
		const bool mixedH = !AreAllEqualValues(hs);

		float x = xs[0];
		float y = ys[0];
		float w = ws[0];
		float h = hs[0];

		float deltaX = 0.0f;
		float deltaY = 0.0f;
		float deltaW = 0.0f;
		float deltaH = 0.0f;

		bool changedX = false;
		bool changedY = false;
		bool changedW = false;
		bool changedH = false;

		ImGui::TextUnformatted(lastField.key.c_str());
		ImGui::SameLine();

		if (!DrawMixedRectFDragFieldPerAxis(
			("##" + lastField.name).c_str(),
			mixedX,
			mixedY,
			mixedW,
			mixedH,
			x,
			y,
			w,
			h,
			deltaX,
			deltaY,
			deltaW,
			deltaH,
			changedX,
			changedY,
			changedW,
			changedH,
			0.1f,
			"%.1f"))
		{
			return false;
		}

		for (buki::RectF* ptr : values)
		{
			if (ptr == nullptr)
				continue;

			if (changedX) ptr->x = x;
			if (changedY) ptr->y = y;
			if (changedW) ptr->w = w;
			if (changedH) ptr->h = h;
		}

		return true;
	}

	bool DrawSharedColorProperty(const buki::PropertyInfo& propInfo, const std::vector<buki::Color*>& values)
	{
		if (values.empty())
			return false;

		std::vector<buki::BukiField> fields = propInfo.prop.fields;
		buki::BukiField lastField = fields[fields.size() - 1];

		std::vector<float> rs;
		std::vector<float> gs;
		std::vector<float> bs;
		std::vector<float> as;
		rs.reserve(values.size());
		gs.reserve(values.size());
		bs.reserve(values.size());
		as.reserve(values.size());

		for (buki::Color* ptr : values)
		{
			const buki::Color color = ptr ? *ptr : buki::Color{};
			rs.push_back(color.r);
			gs.push_back(color.g);
			bs.push_back(color.b);
			as.push_back(color.a);
		}

		const bool mixedR = !AreAllEqualValues(rs);
		const bool mixedG = !AreAllEqualValues(gs);
		const bool mixedB = !AreAllEqualValues(bs);
		const bool mixedA = !AreAllEqualValues(as);

		float r = rs[0];
		float g = gs[0];
		float b = bs[0];
		float a = as[0];

		float deltaR = 0.0f;
		float deltaG = 0.0f;
		float deltaB = 0.0f;
		float deltaA = 0.0f;

		bool changedR = false;
		bool changedG = false;
		bool changedB = false;
		bool changedA = false;

		if (!DrawMixedColorDragDeltaField(
			lastField.name.c_str(),
			mixedR,
			mixedG,
			mixedB,
			mixedA,
			r,
			g,
			b,
			a,
			deltaR,
			deltaG,
			deltaB,
			deltaA,
			changedR,
			changedG,
			changedB,
			changedA))
		{
			return false;
		}

		for (buki::Color* ptr : values)
		{
			if (ptr == nullptr)
				continue;

			if (changedR)
			{
				if (mixedR) ptr->r += deltaR;
				else ptr->r = r;
			}

			if (changedG)
			{
				if (mixedG) ptr->g += deltaG;
				else ptr->g = g;
			}

			if (changedB)
			{
				if (mixedB) ptr->b += deltaB;
				else ptr->b = b;
			}

			if (changedA)
			{
				if (mixedA) ptr->a += deltaA;
				else ptr->a = a;
			}

			ptr->r = std::clamp(ptr->r, 0.0f, 1.0f);
			ptr->g = std::clamp(ptr->g, 0.0f, 1.0f);
			ptr->b = std::clamp(ptr->b, 0.0f, 1.0f);
			ptr->a = std::clamp(ptr->a, 0.0f, 1.0f);
		}

		return true;
	}

	bool DrawSharedEnumIntProperty(const buki::PropertyInfo& propInfo, const std::vector<int*>& values)
	{
		if (values.empty())
			return false;

		std::vector<buki::BukiField> fields = propInfo.prop.fields;
		buki::BukiField lastField = fields[fields.size() - 1];

		std::vector<int> snapshot;
		snapshot.reserve(values.size());
		for (int* ptr : values)
			snapshot.push_back(ptr ? *ptr : 0);

		const bool mixed = !AreAllEqualValues(snapshot);
		int value = snapshot[0];

		if (propInfo.enumLabels.empty())
			return false;

		if (!mixed)
		{
			if (value < 0) value = 0;
			if (value >= static_cast<int>(propInfo.enumLabels.size()))
				value = static_cast<int>(propInfo.enumLabels.size()) - 1;
		}

		bool changed = false;
		const char* preview = mixed ? "-" : propInfo.enumLabels[value].c_str();

		ImGui::Text("%s", lastField.key.c_str());

		if (ImGui::BeginCombo(("##" + lastField.name).c_str(), preview))
		{
			for (int i = 0; i < static_cast<int>(propInfo.enumLabels.size()); ++i)
			{
				const bool selected = (!mixed && value == i);
				if (ImGui::Selectable(propInfo.enumLabels[i].c_str(), selected))
				{
					value = i;
					changed = true;
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (!changed)
			return false;

		ApplySharedValue(values, value);
		return true;
	}

	bool DrawSharedAssetProperty(
		const buki::PropertyInfo& propInfo,
		const std::vector<std::string*>& values,
		const char* payloadType)
	{
		if (values.empty())
			return false;

		std::vector<buki::BukiField> fields = propInfo.prop.fields;
		buki::BukiField lastField = fields[fields.size() - 1];

		std::vector<std::string> snapshot;
		snapshot.reserve(values.size());
		for (std::string* ptr : values)
			snapshot.push_back(ptr ? *ptr : "");

		const bool mixed = !AreAllEqualValues(snapshot);
		std::string path = snapshot[0];
		bool changed = false;

		ImGui::PushID(lastField.key.c_str());

		ImGui::Text("%s", lastField.key.c_str());
		ImGui::SameLine();

		std::string previewPath = mixed ? "" : path;
		const ImGuiID pickerId = ImGui::GetID("AssetPickerWindow");
		FloatingAssetPickerState& state = sAssetPickerStates[pickerId];
		if (DrawInlineAssetPreview(previewPath, payloadType, 44.0f))
		{
			state.open = true;
			state.setInitialPos = true;
			state.initialPos = ImGui::GetItemRectMin();
		}
		if (mixed)
		{
			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::TextUnformatted("-");
			ImGui::TextDisabled("(mixed)");
			ImGui::EndGroup();
		}

		changed |= DrawAssetPickerWindow("Asset Picker##AssetPickerWindow", payloadType, path, state);

		ImGui::PopID();

		if (!changed)
			return false;

		ApplySharedValue(values, path);
		return true;
	}

	bool DrawSharedAssetListProperty(const buki::PropertyInfo& propInfo, const std::vector<std::vector<std::string>*>& values, const char* payloadType)
	{
		if (values.empty())
			return false;

		std::vector<buki::BukiField> fields = propInfo.prop.fields;
		buki::BukiField lastField = fields[fields.size() - 1];

		std::vector<std::string> first = values[0] ? *values[0] : std::vector<std::string>{};
		bool mixed = false;

		for (size_t i = 1; i < values.size(); ++i)
		{
			if (values[i] == nullptr || *values[i] != first)
			{
				mixed = true;
				break;
			}
		}

		ImGui::PushID(lastField.key.c_str());

		std::vector<std::string> editValues = first;

		bool applyRequested = false;
		bool clearRequested = false;
		bool changedInEditor = DrawAssetListEditor(
			lastField.key.c_str(),
			editValues,
			payloadType,
			mixed,
			applyRequested,
			clearRequested);

		bool changed = false;

		if (!mixed && changedInEditor)
		{
			for (std::vector<std::string>* ptr : values)
			{
				if (ptr != nullptr)
					*ptr = editValues;
			}
			changed = true;
		}
		else if (mixed && applyRequested)
		{
			for (std::vector<std::string>* ptr : values)
			{
				if (ptr != nullptr)
					*ptr = editValues;
			}

			changed = true;
		}
		else if (mixed && clearRequested)
		{
			for (std::vector<std::string>* ptr : values)
			{
				if (ptr != nullptr)
					ptr->clear();
			}
			changed = true;
		}

		ImGui::PopID();
		return changed;
	}
	
	bool DrawSharedEntityRefProperty(const buki::PropertyInfo& propInfo, const std::vector<buki::EntityRef*>& refs)
	{
		if (refs.empty())
			return false;

		std::vector<buki::BukiField> fields = propInfo.prop.fields;
		buki::BukiField lastField = fields[fields.size() - 1];

		const bool mixed = !AreAllEqualEntityRefs(refs);
		buki::EntityRef preview = refs[0] ? *refs[0] : buki::EntityRef{};
		bool changed = false;

		ImGui::Text("%s", lastField.key.c_str());
		ImGui::SameLine(180.0f);

		std::string previewText = "None";
		if (mixed)
		{
			previewText = "-";
		}
		else if (preview.cached != nullptr)
		{
			previewText = preview.cached->GetName();
		}
		else if (!preview.entityName.empty())
		{
			previewText = preview.entityName;
		}
		else if (!preview.prefabPath.empty())
		{
			previewText = buki::DisplayNameForPath(preview.prefabPath);
		}

		if (ImGui::Button(previewText.c_str(), ImVec2(220.0f, 0.0f)))
			ImGui::OpenPopup(("SharedEntityPicker##" + lastField.key).c_str());

		if (ImGui::BeginPopup(("SharedEntityPicker##" + lastField.key).c_str()))
		{
			if (ImGui::Selectable("None"))
			{
				for (buki::EntityRef* ref : refs)
				{
					if (ref != nullptr)
						ref->Clear();
				}
				changed = true;
				ImGui::CloseCurrentPopup();
			}

			buki::IWorld& world = buki::Engine::Get().World();
			for (buki::Entity* entity : world.GetEntitiesInWorld())
			{
				if (entity == nullptr)
					continue;

				const std::string entityName = entity->GetName();
				if (ImGui::Selectable(entityName.c_str(), !mixed && preview.entityName == entityName && preview.prefabPath.empty()))
				{
					for (buki::EntityRef* ref : refs)
					{
						if (ref == nullptr)
							continue;

						ref->entityName = entityName;
						ref->prefabPath.clear();
						ref->cached = entity;
					}
					changed = true;
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::Separator();
			ImGui::TextDisabled("Drop an entity or prefab here.");

			ImGui::EndPopup();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
			{
				buki::Entity* droppedEntity = *static_cast<buki::Entity* const*>(payload->Data);
				if (droppedEntity != nullptr)
				{
					for (buki::EntityRef* ref : refs)
					{
						if (ref == nullptr)
							continue;

						ref->entityName = droppedEntity->GetName();
						ref->prefabPath.clear();
						ref->cached = droppedEntity;
					}
					changed = true;
				}
			}

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PREFAB"))
			{
				const char* droppedPath = static_cast<const char*>(payload->Data);
				if (droppedPath != nullptr)
				{
					for (buki::EntityRef* ref : refs)
					{
						if (ref == nullptr)
							continue;

						ref->entityName.clear();
						ref->prefabPath = droppedPath;
						ref->cached = nullptr;
					}
					changed = true;
				}
			}

			ImGui::EndDragDropTarget();
		}

		return changed;
	}
}


bool buki::InspectorPropertyDrawer::DrawComponent(Component* component, std::string* name)
{
	if (component == nullptr || name == nullptr)
		return false;

	bool changed = false;

	if (ImGui::TreeNodeEx(name->c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		const auto& props = component->GetProperties();
		std::string currentGroup;
		for (const PropertyInfo& propInfo : props)
		{
			if (!propInfo.prop.group.empty() && propInfo.prop.group != currentGroup)
			{
				currentGroup = propInfo.prop.group;
				ImGui::SeparatorText(currentGroup.c_str());
			}

			char* base = reinterpret_cast<char*>(component);
			auto fields = propInfo.prop.fields;
			void* fieldPtr = base + fields[fields.size() - 1].offset;
			ImGui::PushID(fields[fields.size() - 1].key.c_str());
			changed |= DrawProperty(propInfo, fieldPtr);
			ImGui::PopID();
		}

		ImGui::TreePop();
	}

	return changed;
}

bool buki::InspectorPropertyDrawer::DrawProperty(const PropertyInfo& propInfo, void* fieldPtr)
{
	std::vector<BukiField> fields = propInfo.prop.fields;
	BukiField lastField = fields[fields.size() - 1];
	ImGui::Text("%s", lastField.key.c_str());

	switch (propInfo.prop.type)
	{
	case PropertyType::Int:               ImGui::SameLine(); return DrawIntProperty(propInfo, fieldPtr);
	case PropertyType::Float:             ImGui::SameLine(); return DrawFloatProperty(propInfo, fieldPtr);
	case PropertyType::Bool:              ImGui::SameLine(); return DrawBoolProperty(propInfo, fieldPtr);
	case PropertyType::String:            ImGui::SameLine(); return DrawStringProperty(propInfo, fieldPtr);
	case PropertyType::StringList:        return DrawStringListProperty(propInfo, fieldPtr);
	case PropertyType::ImageAsset:        ImGui::SameLine(); return DrawStringAssetPicker(propInfo, fieldPtr, "IMAGE");
	case PropertyType::AudioAsset:        ImGui::SameLine(); return DrawStringAssetPicker(propInfo, fieldPtr, "AUDIO");
	case PropertyType::ImageAssetList:    return DrawStringAssetListPicker(propInfo, fieldPtr, "IMAGE");
	case PropertyType::AudioAssetList:    return DrawStringAssetListPicker(propInfo, fieldPtr, "AUDIO");
	case PropertyType::EntityRef:         ImGui::SameLine(); return DrawEntityPicker(propInfo, fieldPtr);
	case PropertyType::PrefabRef:         ImGui::SameLine(); return DrawPrefabPicker(propInfo, fieldPtr);
	case PropertyType::FontAsset:         ImGui::SameLine(); return DrawStringAssetPicker(propInfo, fieldPtr, "FONT");
	case PropertyType::FontAssetList:     return DrawStringAssetListPicker(propInfo, fieldPtr, "FONT");
	case PropertyType::Vector2:           ImGui::SameLine(); return DrawVector2Property(propInfo, fieldPtr);
	case PropertyType::Color:             ImGui::SameLine(); return DrawColorProperty(propInfo, fieldPtr);
	case PropertyType::RectF:             ImGui::SameLine(); return DrawRectFProperty(propInfo, fieldPtr);
	case PropertyType::EnumInt:           return DrawEnumIntProperty(propInfo, fieldPtr);
	case PropertyType::EnumScriptBinding: ImGui::SameLine(); return DrawEnumScriptBindingProperty(propInfo, fieldPtr);
	case PropertyType::TileList:          return DrawTileListProperty(propInfo, fieldPtr);
	default:                              return false;
	}
}

bool buki::InspectorPropertyDrawer::DrawEntitySection(Entity* entity)
{
	if (entity == nullptr)
		return false;

	bool changed = false;

	if (ImGui::CollapsingHeader("Entity", ImGuiTreeNodeFlags_DefaultOpen))
	{
		char nameBuffer[256] = {};
		std::snprintf(nameBuffer, sizeof(nameBuffer), "%s", entity->GetName().c_str());

		if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
		{
			if (buki::Engine::Get().World().RenameEntity(entity, nameBuffer))
				changed = true;
		}

		bool enabled = entity->IsEnabled();
		ImGui::TextUnformatted("Enabled");
		ImGui::SameLine();
		if (DrawMixedBoolField("##Enabled", false, enabled))
		{
			entity->SetEnable(enabled);
			changed = true;
		}

		if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& t = entity->Tm();

			Vector2 position = t.GetPosition();
			float posX = position.x;
			float posY = position.y;
			float deltaPosX = 0.0f;
			float deltaPosY = 0.0f;
			bool changedPosX = false;
			bool changedPosY = false;

			ImGui::TextUnformatted("Position");
			ImGui::SameLine();
			if (DrawMixedVector2DragDeltaFieldPerAxis(
				"##Position",
				false,
				false,
				posX,
				posY,
				deltaPosX,
				deltaPosY,
				changedPosX,
				changedPosY,
				"%.1f"))
			{
				Vector2 value = t.GetPosition();
				if (changedPosX) value.x = posX;
				if (changedPosY) value.y = posY;
				t.SetPosition(value);
				changed = true;
			}

			Vector2 size = t.GetSize();
			float sizeX = size.x;
			float sizeY = size.y;
			float deltaSizeX = 0.0f;
			float deltaSizeY = 0.0f;
			bool changedSizeX = false;
			bool changedSizeY = false;

			ImGui::TextUnformatted("Size");
			ImGui::SameLine();
			if (DrawMixedVector2DragDeltaFieldPerAxis(
				"##Size",
				false,
				false,
				sizeX,
				sizeY,
				deltaSizeX,
				deltaSizeY,
				changedSizeX,
				changedSizeY,
				"%.1f"))
			{
				Vector2 value = t.GetSize();
				if (changedSizeX) value.x = sizeX;
				if (changedSizeY) value.y = sizeY;
				t.SetSize(value);
				changed = true;
			}

			float rotation = t.GetRotation().GetRadians();
			float deltaRotation = 0.0f;

			ImGui::TextUnformatted("Rotation");
			ImGui::SameLine();
			if (DrawMixedFloatDragDeltaField(
				"##Rotation",
				false,
				rotation,
				deltaRotation,
				0.1f,
				"%.1f"))
			{
				t.SetRotation(rotation);
				changed = true;
			}

			ImGui::TreePop();
		}

		int z = entity->GetZ();
		int deltaZ = 0;
		ImGui::TextUnformatted("Z");
		ImGui::SameLine();
		if (DrawMixedIntDragDeltaField("##Z", false, z, deltaZ))
		{
			entity->SetZ(z);
			Engine::Get().World().SortEntities();
			changed = true;
		}

		std::string layer = entity->GetLayer();
		ImGui::TextUnformatted("Layer");
		ImGui::SameLine();
		if (DrawMixedStringEditor("##Layer", false, layer))
		{
			entity->SetLayer(layer);
			changed = true;
		}
	}

	return changed;
}

bool buki::InspectorPropertyDrawer::DrawMultiEntitySection(const std::vector<Entity*>& entities)
{
	if (entities.empty())
		return false;

	Entity* firstValid = nullptr;
	for (Entity* entity : entities)
	{
		if (entity != nullptr)
		{
			firstValid = entity;
			break;
		}
	}

	if (firstValid == nullptr)
		return false;

	bool changed = false;

	if (ImGui::CollapsingHeader("Entity", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool allEnabled = true;
		bool anyEnabled = false;

		for (Entity* entity : entities)
		{
			if (entity == nullptr)
				continue;

			if (entity->IsEnabled())
				anyEnabled = true;
			else
				allEnabled = false;
		}

		bool enabledValue = allEnabled;
		const bool mixedEnabled = (allEnabled != anyEnabled);

		ImGui::TextUnformatted("Enabled");
		ImGui::SameLine();
		if (DrawMixedBoolField("##Enabled", mixedEnabled, enabledValue))
		{
			for (Entity* entity : entities)
			{
				if (entity != nullptr)
					entity->SetEnable(enabledValue);
			}
			changed = true;
		}

		changed |= DrawMultiTransformSection(entities);

		const int firstZ = firstValid->GetZ();
		bool mixedZ = false;
		for (Entity* entity : entities)
		{
			if (entity != nullptr && entity->GetZ() != firstZ)
			{
				mixedZ = true;
				break;
			}
		}

		int z = firstZ;
		int deltaZ = 0;
		ImGui::TextUnformatted("Z");
		ImGui::SameLine();
		if (DrawMixedIntDragDeltaField("##Z", mixedZ, z, deltaZ))
		{
			for (Entity* entity : entities)
			{
				if (entity != nullptr)
					entity->SetZ(z);
			}

			Engine::Get().World().SortEntities();
			changed = true;
		}

		std::string firstLayer = firstValid->GetLayer();
		bool mixedLayer = false;
		for (Entity* entity : entities)
		{
			if (entity != nullptr && entity->GetLayer() != firstLayer)
			{
				mixedLayer = true;
				break;
			}
		}

		std::string layer = firstLayer;
		ImGui::TextUnformatted("Layer");
		ImGui::SameLine();
		if (DrawMixedStringEditor("##Layer", mixedLayer, layer))
		{
			for (Entity* entity : entities)
			{
				if (entity != nullptr)
					entity->SetLayer(layer);
			}
			changed = true;
		}
	}

	return changed;
}

bool buki::InspectorPropertyDrawer::DrawMultiTransformSection(const std::vector<Entity*>& entities)
{
	if (entities.empty())
		return false;

	Entity* firstValid = nullptr;
	for (Entity* entity : entities)
	{
		if (entity != nullptr)
		{
			firstValid = entity;
			break;
		}
	}

	if (firstValid == nullptr)
		return false;

	bool changed = false;

	if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const Vector2 firstPosition = firstValid->Tm().GetPosition();
		const Vector2 firstSize = firstValid->Tm().GetSize();
		const float firstRotation = firstValid->Tm().GetRotation().GetRadians();

		std::vector<Vector2> positions;
		std::vector<Vector2> sizes;
		positions.reserve(entities.size());
		sizes.reserve(entities.size());

		bool mixedRotation = false;

		for (Entity* entity : entities)
		{
			if (entity == nullptr)
				continue;

			const Vector2 position = entity->Tm().GetPosition();
			const Vector2 size = entity->Tm().GetSize();
			const float rotation = entity->Tm().GetRotation().GetRadians();

			positions.push_back(position);
			sizes.push_back(size);

			if (rotation != firstRotation)
				mixedRotation = true;
		}

		const bool mixedPosX = !AreAllEqualVector2X(positions);
		const bool mixedPosY = !AreAllEqualVector2Y(positions);

		float posX = firstPosition.x;
		float posY = firstPosition.y;
		float deltaPosX = 0.0f;
		float deltaPosY = 0.0f;
		bool changedPosX = false;
		bool changedPosY = false;

		ImGui::TextUnformatted("Position");
		ImGui::SameLine();
		if (DrawMixedVector2DragDeltaFieldPerAxis(
			"##Position",
			mixedPosX,
			mixedPosY,
			posX,
			posY,
			deltaPosX,
			deltaPosY,
			changedPosX,
			changedPosY,
			"%.1f"))
		{
			for (Entity* entity : entities)
			{
				if (entity == nullptr)
					continue;

				auto& t = entity->Tm();
				Vector2 value = t.GetPosition();

				if (changedPosX)
				{
					if (mixedPosX) value.x += deltaPosX;
					else value.x = posX;
				}

				if (changedPosY)
				{
					if (mixedPosY) value.y += deltaPosY;
					else value.y = posY;
				}

				t.SetPosition(value);
			}

			changed = true;
		}

		const bool mixedSizeX = !AreAllEqualVector2X(sizes);
		const bool mixedSizeY = !AreAllEqualVector2Y(sizes);

		float sizeX = firstSize.x;
		float sizeY = firstSize.y;
		float deltaSizeX = 0.0f;
		float deltaSizeY = 0.0f;
		bool changedSizeX = false;
		bool changedSizeY = false;

		ImGui::TextUnformatted("Size");
		ImGui::SameLine();
		if (DrawMixedVector2DragDeltaFieldPerAxis(
			"##Size",
			mixedSizeX,
			mixedSizeY,
			sizeX,
			sizeY,
			deltaSizeX,
			deltaSizeY,
			changedSizeX,
			changedSizeY,
			"%.1f"))
		{
			for (Entity* entity : entities)
			{
				if (entity == nullptr)
					continue;

				auto& t = entity->Tm();
				Vector2 value = t.GetSize();

				if (changedSizeX)
				{
					if (mixedSizeX) value.x += deltaSizeX;
					else value.x = sizeX;
				}

				if (changedSizeY)
				{
					if (mixedSizeY) value.y += deltaSizeY;
					else value.y = sizeY;
				}

				t.SetSize(value);
			}

			changed = true;
		}

		float rotation = firstRotation;
		float deltaRotation = 0.0f;

		ImGui::TextUnformatted("Rotation");
		ImGui::SameLine();
		if (DrawMixedFloatDragDeltaField(
			"##Rotation",
			mixedRotation,
			rotation,
			deltaRotation,
			0.1f,
			"%.1f"))
		{
			for (Entity* entity : entities)
			{
				if (entity == nullptr)
					continue;

				auto& t = entity->Tm();

				if (mixedRotation)
				{
					const float currentRotation = t.GetRotation().GetRadians();
					t.SetRotation(currentRotation + deltaRotation);
				}
				else
				{
					t.SetRotation(rotation);
				}
			}

			changed = true;
		}

		ImGui::TreePop();
	}

	return changed;
}

bool buki::InspectorPropertyDrawer::DrawIntProperty(const PropertyInfo& propInfo, void* fieldPtr)
{
	std::vector<BukiField> fields = propInfo.prop.fields;
	BukiField lastField = fields[fields.size() - 1];
	int* value = reinterpret_cast<int*>(fieldPtr);
	if (value == nullptr)
		return false;

	bool changed = false;

	if (propInfo.hasMin && propInfo.hasMax)
		changed = ImGui::SliderInt(("##" + lastField.key).c_str(), value, static_cast<int>(propInfo.minValue), static_cast<int>(propInfo.maxValue));
	else
		changed = ImGui::DragInt(("##" + lastField.key).c_str(), value, propInfo.dragSpeed);

	if (!changed)
		return false;

	if (propInfo.hasMin && *value < static_cast<int>(propInfo.minValue))
		*value = static_cast<int>(propInfo.minValue);
	if (propInfo.hasMax && *value > static_cast<int>(propInfo.maxValue))
		*value = static_cast<int>(propInfo.maxValue);

	return true;
}

bool buki::InspectorPropertyDrawer::DrawFloatProperty(const PropertyInfo& propInfo, void* fieldPtr)
{
	std::vector<BukiField> fields = propInfo.prop.fields;
	BukiField lastField = fields[fields.size() - 1];
	float* value = reinterpret_cast<float*>(fieldPtr);
	if (value == nullptr)
		return false;

	bool changed = false;

	if (propInfo.hasMin && propInfo.hasMax)
		changed = ImGui::SliderFloat(("##" + lastField.key).c_str(), value, propInfo.minValue, propInfo.maxValue);
	else
		changed = ImGui::DragFloat(("##" + lastField.key).c_str(), value, propInfo.dragSpeed);

	if (!changed)
		return false;

	if (propInfo.hasMin && *value < propInfo.minValue)
		*value = propInfo.minValue;
	if (propInfo.hasMax && *value > propInfo.maxValue)
		*value = propInfo.maxValue;

	return true;
}

bool buki::InspectorPropertyDrawer::DrawBoolProperty(const PropertyInfo& propInfo, void* fieldPtr)
{
	std::vector<BukiField> fields = propInfo.prop.fields;
	BukiField lastField = fields[fields.size() - 1];
	bool* value = reinterpret_cast<bool*>(fieldPtr);
	if (value == nullptr)
		return false;
	const bool result = ImGui::Checkbox(("##" + lastField.key).c_str(), value);
	return result;
}

bool buki::InspectorPropertyDrawer::DrawStringProperty(const PropertyInfo& propInfo, void* fieldPtr)
{
	std::vector<BukiField> fields = propInfo.prop.fields;
	BukiField lastField = fields[fields.size() - 1];
	std::string* value = reinterpret_cast<std::string*>(fieldPtr);
	if (value == nullptr)
		return false;

	char buffer[256] = {};
	strncpy_s(buffer, value->c_str(), sizeof(buffer) - 1);

	if (!ImGui::InputText(("##" + lastField.key).c_str(), buffer, sizeof(buffer)))
		return false;

	*value = buffer;
	return true;
}

bool buki::InspectorPropertyDrawer::DrawStringListProperty(const PropertyInfo& propInfo, void* fieldPtr)
{
	std::vector<BukiField> fields = propInfo.prop.fields;
	BukiField lastField = fields[fields.size() - 1];
	std::vector<std::string>* values = reinterpret_cast<std::vector<std::string>*>(fieldPtr);
	if (values == nullptr)
		return false;

	bool changed = false;
	int removeIndex = -1;

	for (int i = 0; i < static_cast<int>(values->size()); ++i)
	{
		ImGui::PushID(i);

		char buffer[256] = {};
		strncpy_s(buffer, (*values)[i].c_str(), sizeof(buffer) - 1);

		ImGui::SetNextItemWidth(220.0f);
		if (ImGui::InputText(("##value" + std::to_string(i)).c_str(), buffer, sizeof(buffer)))
		{
			(*values)[i] = buffer;
			changed = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("-"))
			removeIndex = i;

		ImGui::PopID();
	}

	if (removeIndex >= 0)
	{
		values->erase(values->begin() + removeIndex);
		changed = true;
	}

	if (ImGui::Button(("Add##" + lastField.key).c_str()))
	{
		values->push_back("");
		changed = true;
	}

	ImGui::Unindent();
	return changed;
}

bool buki::InspectorPropertyDrawer::DrawStringAssetPicker(const PropertyInfo& propInfo, void* fieldPtr, const char* payloadType)
{
	std::vector<BukiField> fields = propInfo.prop.fields;
	BukiField lastField = fields[fields.size() - 1];
	std::string* path = reinterpret_cast<std::string*>(fieldPtr);
	if (path == nullptr)
		return false;

	bool changed = false;

	ImGui::PushID(lastField.key.c_str());

	const ImGuiID pickerId = ImGui::GetID("AssetPickerWindow");
	FloatingAssetPickerState& state = sAssetPickerStates[pickerId];

	if (DrawInlineAssetPreview(*path, payloadType, 44.0f))
	{
		state.open = true;
		state.setInitialPos = true;
		state.initialPos = ImGui::GetItemRectMax();
	}

	changed |= DrawAssetPickerWindow("Asset Picker##AssetPickerWindow", payloadType, *path, state);

	ImGui::PopID();
	return changed;
}

bool buki::InspectorPropertyDrawer::DrawStringAssetListPicker(const PropertyInfo& propInfo, void* fieldPtr, const char* payloadType)
{
	std::vector<BukiField> fields = propInfo.prop.fields;
	BukiField lastField = fields[fields.size() - 1];
	std::vector<std::string>* values = reinterpret_cast<std::vector<std::string>*>(fieldPtr);
	if (values == nullptr)
		return false;

	ImGui::PushID(lastField.key.c_str());
	bool applyRequested = false;
	bool clearRequested = false;
	bool changed = DrawAssetListEditor(("##" + lastField.key).c_str(), *values, payloadType, false, applyRequested, clearRequested);

	if (clearRequested)
	{
		values->clear();
		changed = true;
	}


	ImGui::PopID();
	return changed;
}

bool buki::InspectorPropertyDrawer::DrawEntityPicker(const PropertyInfo& propInfo, void* fieldPtr)
{
	std::vector<BukiField> fields = propInfo.prop.fields;
	BukiField lastField = fields[fields.size() - 1];
	EntityRef* ref = reinterpret_cast<EntityRef*>(fieldPtr);
	if (ref == nullptr)
		return false;

	bool changed = false;

	std::string previewText = "None";
	if (ref->cached != nullptr)
		previewText = ref->cached->GetName();
	else if (!ref->entityName.empty())
		previewText = ref->entityName;
	else if (!ref->prefabPath.empty())
		previewText = DisplayNameForPath(ref->prefabPath);

	if (ImGui::Button(previewText.c_str(), ImVec2(220.0f, 0.0f)))
		ImGui::OpenPopup(("EntityPicker##" + lastField.key).c_str());

	if (ImGui::BeginPopup(("EntityPicker##" + lastField.key).c_str()))
	{
		if (ImGui::Selectable("None"))
		{
			ref->Clear();
			changed = true;
		}

		IWorld& world = Engine::Get().World();
		for (Entity* entity : world.GetEntitiesInWorld())
		{
			if (entity == nullptr)
				continue;

			const std::string entityName = entity->GetName();
			const bool selected = (ref->entityName == entityName && ref->prefabPath.empty());

			if (ImGui::Selectable(entityName.c_str(), selected))
			{
				ref->entityName = entityName;
				ref->prefabPath.clear();
				ref->cached = entity;
				changed = true;
			}
		}

		ImGui::Separator();
		ImGui::TextDisabled("Drop an entity or prefab here.");

		ImGui::EndPopup();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
		{
			Entity* droppedEntity = *static_cast<Entity* const*>(payload->Data);
			if (droppedEntity != nullptr)
			{
				ref->entityName = droppedEntity->GetName();
				ref->prefabPath.clear();
				ref->cached = droppedEntity;
				changed = true;
			}
		}

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PREFAB"))
		{
			const char* droppedPath = static_cast<const char*>(payload->Data);
			if (droppedPath != nullptr)
			{
				ref->entityName.clear();
				ref->prefabPath = droppedPath;
				ref->cached = nullptr;
				changed = true;
			}
		}

		ImGui::EndDragDropTarget();
	}

	return changed;
}

bool buki::InspectorPropertyDrawer::DrawPrefabPicker(const PropertyInfo& propInfo, void* fieldPtr)
{
	return DrawStringAssetPicker(propInfo, fieldPtr, "PREFAB");
}

bool buki::InspectorPropertyDrawer::DrawVector2Property(const PropertyInfo& propInfo, void* fieldPtr)
{
	Vector2* value = reinterpret_cast<Vector2*>(fieldPtr);
	if (value == nullptr)
		return false;

	std::vector<BukiField> fields = propInfo.prop.fields;
	if (fields.empty())
		return false;

	const BukiField& lastField = fields.back();

	float x = value->x;
	float y = value->y;
	const float speed = propInfo.dragSpeed > 0.0f ? propInfo.dragSpeed : 0.1f;

	ImGui::PushID(lastField.name.c_str());

	const float totalWidth = ImGui::GetContentRegionAvail().x;
	const float labelWidth = 14.0f;
	const float spacing = ImGui::GetStyle().ItemSpacing.x;
	const float fieldWidth = std::max(20.0f, (totalWidth - (labelWidth * 2.0f) - (spacing * 3.0f)) * 0.5f);

	bool changed = false;

	ImGui::TextUnformatted("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(fieldWidth);
	changed |= ImGui::DragFloat(("##" + lastField.name + "_X").c_str(), &x, speed, 0.0f, 0.0f, "%.1f");

	ImGui::SameLine();
	ImGui::TextUnformatted("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(fieldWidth);
	changed |= ImGui::DragFloat(("##" + lastField.name + "_Y").c_str(), &y, speed, 0.0f, 0.0f, "%.1f");

	ImGui::PopID();

	if (!changed)
		return false;

	value->x = x;
	value->y = y;
	return true;
}

bool buki::InspectorPropertyDrawer::DrawColorProperty(const PropertyInfo& propInfo, void* fieldPtr)
{
	Color* value = reinterpret_cast<Color*>(fieldPtr);
	if (value == nullptr)
		return false;

	std::vector<BukiField> fields = propInfo.prop.fields;
	if (fields.empty())
		return false;

	const BukiField& lastField = fields.back();

	float v[4] = { value->r, value->g, value->b, value->a };

	ImGui::PushID(lastField.name.c_str());
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

	const bool changed = ImGui::ColorEdit4(("##" + lastField.name).c_str(), v);
	ImGui::PopID();

	if (!changed)
		return false;

	value->r = v[0];
	value->g = v[1];
	value->b = v[2];
	value->a = v[3];
	return true;
}

bool buki::InspectorPropertyDrawer::DrawRectFProperty(const PropertyInfo& propInfo, void* fieldPtr)
{
	RectF* value = reinterpret_cast<RectF*>(fieldPtr);
	if (value == nullptr)
		return false;

	std::vector<BukiField> fields = propInfo.prop.fields;
	if (fields.empty())
		return false;

	const BukiField& lastField = fields.back();

	float x = value->x;
	float y = value->y;
	float w = value->w;
	float h = value->h;

	const float speed = propInfo.dragSpeed > 0.0f ? propInfo.dragSpeed : 0.1f;

	ImGui::PushID(lastField.name.c_str());

	bool changed = false;

	const float avail = ImGui::GetContentRegionAvail().x;
	const float textWidth = 14.0f;
	const float spacing = ImGui::GetStyle().ItemSpacing.x;
	const bool compact = avail < 260.0f;

	if (compact)
	{
		const float rowFieldWidth = std::max(20.0f, (avail - (textWidth * 2.0f) - (spacing * 3.0f)) * 0.5f);

		ImGui::TextUnformatted("X");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(rowFieldWidth);
		changed |= ImGui::DragFloat(("##" + lastField.name + "_X").c_str(), &x, speed, 0.0f, 0.0f, "%.1f");

		ImGui::SameLine();
		ImGui::TextUnformatted("Y");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(rowFieldWidth);
		changed |= ImGui::DragFloat(("##" + lastField.name + "_Y").c_str(), &y, speed, 0.0f, 0.0f, "%.1f");

		ImGui::TextUnformatted("W");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(rowFieldWidth);
		changed |= ImGui::DragFloat(("##" + lastField.name + "_W").c_str(), &w, speed, 0.0f, 0.0f, "%.1f");

		ImGui::SameLine();
		ImGui::TextUnformatted("H");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(rowFieldWidth);
		changed |= ImGui::DragFloat(("##" + lastField.name + "_H").c_str(), &h, speed, 0.0f, 0.0f, "%.1f");
	}
	else
	{
		ImGui::SameLine();

		const float fieldWidth = std::max(
			18.0f,
			(avail - (textWidth * 4.0f) - (spacing * 7.0f)) * 0.25f);

		ImGui::TextUnformatted("X");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(fieldWidth);
		changed |= ImGui::DragFloat(("##" + lastField.name + "_X").c_str(), &x, speed, 0.0f, 0.0f, "%.1f");

		ImGui::SameLine();
		ImGui::TextUnformatted("Y");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(fieldWidth);
		changed |= ImGui::DragFloat(("##" + lastField.name + "_Y").c_str(), &y, speed, 0.0f, 0.0f, "%.1f");

		ImGui::SameLine();
		ImGui::TextUnformatted("W");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(fieldWidth);
		changed |= ImGui::DragFloat(("##" + lastField.name + "_W").c_str(), &w, speed, 0.0f, 0.0f, "%.1f");

		ImGui::SameLine();
		ImGui::TextUnformatted("H");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(fieldWidth);
		changed |= ImGui::DragFloat(("##" + lastField.name + "_H").c_str(), &h, speed, 0.0f, 0.0f, "%.1f");
	}

	ImGui::PopID();

	if (!changed)
		return false;

	value->x = x;
	value->y = y;
	value->w = w;
	value->h = h;
	return true;
}

bool buki::InspectorPropertyDrawer::DrawEnumIntProperty(const PropertyInfo& propInfo, void* fieldPtr)
{
	int* value = reinterpret_cast<int*>(fieldPtr);
	if (value == nullptr)
		return false;

	std::vector<BukiField> fields = propInfo.prop.fields;
	if (fields.empty())
		return false;

	const BukiField& lastField = fields.back();

	ImGui::PushID(lastField.name.c_str());
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

	if (propInfo.enumLabels.empty())
	{
		const bool changed = ImGui::InputInt(("##" + lastField.name).c_str(), value);
		ImGui::PopID();
		return changed;
	}

	if (*value < 0)
		*value = 0;
	if (*value >= static_cast<int>(propInfo.enumLabels.size()))
		*value = static_cast<int>(propInfo.enumLabels.size()) - 1;

	const char* preview = propInfo.enumLabels[*value].c_str();
	bool changed = false;

	if (ImGui::BeginCombo(("##" + lastField.name).c_str(), preview))
	{
		for (int i = 0; i < static_cast<int>(propInfo.enumLabels.size()); ++i)
		{
			const bool selected = (*value == i);
			if (ImGui::Selectable(propInfo.enumLabels[i].c_str(), selected))
			{
				*value = i;
				changed = true;
			}

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::PopID();
	return changed;
}

bool buki::InspectorPropertyDrawer::DrawEnumScriptBindingProperty(const PropertyInfo& propInfo, void* fieldPtr)
{
	StaticFunctionBinding* value = reinterpret_cast<StaticFunctionBinding*>(fieldPtr);
	if (value == nullptr)
		return false;

	std::vector<BukiField> fields = propInfo.prop.fields;
	if (fields.empty())
		return false;

	const BukiField& lastField = fields.back();

	std::string& scriptName = value->scriptTypeName;
	std::string& functionName = value->functionName;

	bool changed = false;

	ImGui::PushID(lastField.name.c_str());

	const float width = ImGui::GetContentRegionAvail().x;

	const std::string scriptPreview = scriptName.empty() ? "None" : scriptName;
	ImGui::SetNextItemWidth(width);
	if (ImGui::BeginCombo(("##" + lastField.name + "_Script").c_str(), scriptPreview.c_str()))
	{
		const bool noneSelected = scriptName.empty();
		if (ImGui::Selectable("None", noneSelected))
		{
			if (!scriptName.empty() || !functionName.empty())
			{
				scriptName.clear();
				functionName.clear();
				changed = true;
			}
		}

		for (const std::string& typeName : ScriptFunctionRegistry::Get().GetFunctionsScriptsName())
		{
			const bool isSelected = (scriptName == typeName);
			if (ImGui::Selectable(typeName.c_str(), isSelected))
			{
				if (scriptName != typeName)
				{
					scriptName = typeName;
					functionName.clear();
					changed = true;
				}
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	const bool hasValidScript = !scriptName.empty();
	if (!hasValidScript)
		ImGui::BeginDisabled();

	const std::string functionPreview = functionName.empty() ? "None" : functionName;
	ImGui::SetNextItemWidth(width);
	if (ImGui::BeginCombo(("##" + lastField.name + "_Function").c_str(), functionPreview.c_str()))
	{
		const bool noneSelected = functionName.empty();
		if (ImGui::Selectable("None", noneSelected))
		{
			if (!functionName.empty())
			{
				functionName.clear();
				changed = true;
			}
		}

		if (hasValidScript)
		{
			const std::vector<std::string> functionNames =
				ScriptFunctionRegistry::Get().GetFunctionsForScriptName(scriptName);

			for (const std::string& candidate : functionNames)
			{
				const bool isSelected = (functionName == candidate);
				if (ImGui::Selectable(candidate.c_str(), isSelected))
				{
					if (functionName != candidate)
					{
						functionName = candidate;
						changed = true;
					}
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	if (!hasValidScript)
		ImGui::EndDisabled();

	ImGui::PopID();
	return changed;
}

bool buki::InspectorPropertyDrawer::DrawTileProperty(
	const PropertyInfo& propInfo,
	std::vector<TileDrawData>& tiles,
	const size_t index,
	std::vector<size_t>& selectedTiles,
	bool& openRectWindowRequested,
	ImVec2& rectWindowAnchor)
{
	if (index >= tiles.size())
		return false;

	TileDrawData& value = tiles[index];
	bool changed = false;

	auto ApplyToSelectedTiles = [&](auto&& fn)
		{
			for (size_t selectedIndex : selectedTiles)
			{
				if (selectedIndex < tiles.size())
					fn(tiles[selectedIndex]);
			}
		};

	auto IsSelected = [&](size_t i) -> bool
		{
			return std::find(selectedTiles.begin(), selectedTiles.end(), i) != selectedTiles.end();
		};

	auto RemoveSelection = [&](size_t i)
		{
			selectedTiles.erase(
				std::remove(selectedTiles.begin(), selectedTiles.end(), i),
				selectedTiles.end());
		};

	ImGui::PushID(static_cast<int>(index));

	const bool isSelected = IsSelected(index);

	ImGuiTreeNodeFlags headerFlags =
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_AllowOverlap;

	const std::string tileLabel = "Tile " + std::to_string(index);
	bool open = ImGui::TreeNodeEx(tileLabel.c_str(), headerFlags);

	ImVec2 itemMin = ImGui::GetItemRectMin();
	ImVec2 itemMax = ImGui::GetItemRectMax();

	if (isSelected)
	{
		ImDrawList* dl = ImGui::GetWindowDrawList();
		dl->AddRectFilled(itemMin, itemMax, IM_COL32(70, 140, 255, 90), 3.0f);
	}

	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
	{
		const bool ctrl = ImGui::GetIO().KeyCtrl;
		const bool shift = ImGui::GetIO().KeyShift;

		if (shift && !selectedTiles.empty())
		{
			size_t anchor = selectedTiles.back();
			size_t begin = std::min(anchor, index);
			size_t end = std::max(anchor, index);

			selectedTiles.clear();
			for (size_t i = begin; i <= end; ++i)
				selectedTiles.push_back(i);
		}
		else if (ctrl)
		{
			if (IsSelected(index))
				RemoveSelection(index);
			else
				selectedTiles.push_back(index);
		}
		else
		{
			selectedTiles.clear();
			selectedTiles.push_back(index);
		}
	}

	if (selectedTiles.empty())
		selectedTiles.push_back(index);

	const bool tileIsSelectedNow = IsSelected(index);

	if (open)
	{
		const float defaultSpeed = 0.1f;

		ImGui::TextUnformatted("Local Position");
		ImGui::SameLine();
		{
			float x = value.localPosition.x;
			float y = value.localPosition.y;
			const float totalWidth = ImGui::GetContentRegionAvail().x;
			const float axisWidth = 14.0f;
			const float spacing = ImGui::GetStyle().ItemSpacing.x;
			const float fieldWidth = std::max(20.0f, (totalWidth - (axisWidth * 2.0f) - (spacing * 3.0f)) * 0.5f);
			bool edited = false;

			ImGui::TextUnformatted("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			edited |= ImGui::DragFloat("##LocalPositionX", &x, defaultSpeed, 0.0f, 0.0f, "%.3f");

			ImGui::SameLine();
			ImGui::TextUnformatted("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			edited |= ImGui::DragFloat("##LocalPositionY", &y, defaultSpeed, 0.0f, 0.0f, "%.3f");

			if (edited)
			{
				const Vector2 newValue = { x, y };
				if (tileIsSelectedNow)
					ApplyToSelectedTiles([&](TileDrawData& tile) { tile.localPosition = newValue; });
				else
					value.localPosition = newValue;

				changed = true;
			}
		}

		ImGui::TextUnformatted("Size");
		ImGui::SameLine();
		{
			float x = value.size.x;
			float y = value.size.y;
			const float totalWidth = ImGui::GetContentRegionAvail().x;
			const float axisWidth = 14.0f;
			const float spacing = ImGui::GetStyle().ItemSpacing.x;
			const float fieldWidth = std::max(20.0f, (totalWidth - (axisWidth * 2.0f) - (spacing * 3.0f)) * 0.5f);
			bool edited = false;

			ImGui::TextUnformatted("X");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			edited |= ImGui::DragFloat("##SizeX", &x, defaultSpeed, 0.0f, 0.0f, "%.3f");

			ImGui::SameLine();
			ImGui::TextUnformatted("Y");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(fieldWidth);
			edited |= ImGui::DragFloat("##SizeY", &y, defaultSpeed, 0.0f, 0.0f, "%.3f");

			if (edited)
			{
				const Vector2 newValue = { x, y };
				if (tileIsSelectedNow)
					ApplyToSelectedTiles([&](TileDrawData& tile) { tile.size = newValue; });
				else
					value.size = newValue;

				changed = true;
			}
		}

		std::string imagePath = value.texturePath;
		if (DrawInlineAssetPreview(imagePath, "IMAGE", 44.0f, value.sourceRectPixels))
		{
			if (!tileIsSelectedNow)
			{
				selectedTiles.clear();
				selectedTiles.push_back(index);
			}

			openRectWindowRequested = true;
			rectWindowAnchor = ImGui::GetItemRectMin();
		}

		buki::Texture2D* texture = buki::Engine::Get().Textures().Load(imagePath);
		const int texW = texture ? texture->width : 0;
		const int texH = texture ? texture->height : 0;

		ImGui::TextUnformatted("Source Rect");
		{
			int src[4] =
			{
				static_cast<int>(value.sourceRectPixels.x),
				static_cast<int>(value.sourceRectPixels.y),
				static_cast<int>(value.sourceRectPixels.w),
				static_cast<int>(value.sourceRectPixels.h)
			};

			const float avail = ImGui::GetContentRegionAvail().x;
			const float axisWidth = 14.0f;
			const float spacing = ImGui::GetStyle().ItemSpacing.x;
			const bool compact = avail < 220.0f;
			bool edited = false;

			if (compact)
			{
				const float fieldWidth = std::max(20.0f, (avail - (axisWidth * 2.0f) - (spacing * 3.0f)) * 0.5f);

				ImGui::TextUnformatted("X");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(fieldWidth);
				edited |= ImGui::DragInt("##SrcX", &src[0], 1.0f, 0, 0);

				ImGui::SameLine();
				ImGui::TextUnformatted("Y");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(fieldWidth);
				edited |= ImGui::DragInt("##SrcY", &src[1], 1.0f, 0, 0);

				ImGui::TextUnformatted("W");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(fieldWidth);
				edited |= ImGui::DragInt("##SrcW", &src[2], 1.0f, 0, 0);

				ImGui::SameLine();
				ImGui::TextUnformatted("H");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(fieldWidth);
				edited |= ImGui::DragInt("##SrcH", &src[3], 1.0f, 0, 0);
			}
			else
			{
				ImGui::SameLine();
				const float fieldWidth = std::max(18.0f, (avail - (axisWidth * 4.0f) - (spacing * 7.0f)) * 0.25f);

				ImGui::TextUnformatted("X");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(fieldWidth);
				edited |= ImGui::DragInt("##SrcX", &src[0], 1.0f, 0, 0);

				ImGui::SameLine();
				ImGui::TextUnformatted("Y");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(fieldWidth);
				edited |= ImGui::DragInt("##SrcY", &src[1], 1.0f, 0, 0);

				ImGui::SameLine();
				ImGui::TextUnformatted("W");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(fieldWidth);
				edited |= ImGui::DragInt("##SrcW", &src[2], 1.0f, 0, 0);

				ImGui::SameLine();
				ImGui::TextUnformatted("H");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(fieldWidth);
				edited |= ImGui::DragInt("##SrcH", &src[3], 1.0f, 0, 0);
			}

			if (edited)
			{
				buki::RectF newRect =
				{
					static_cast<float>(src[0]),
					static_cast<float>(src[1]),
					static_cast<float>(src[2]),
					static_cast<float>(src[3])
				};

				ClampRectToImage(newRect, static_cast<float>(texW), static_cast<float>(texH));

				if (tileIsSelectedNow)
					ApplyToSelectedTiles([&](TileDrawData& tile) { tile.sourceRectPixels = newRect; });
				else
					value.sourceRectPixels = newRect;

				changed = true;
			}
		}

		float tileColor[4] = { value.color.r, value.color.g, value.color.b, value.color.a };
		ImGui::TextUnformatted("Color");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::ColorEdit4("##Color", tileColor))
		{
			const buki::Color newColor = { tileColor[0], tileColor[1], tileColor[2], tileColor[3] };

			if (tileIsSelectedNow)
				ApplyToSelectedTiles([&](TileDrawData& tile) { tile.color = newColor; });
			else
				value.color = newColor;

			changed = true;
		}

		bool flipX = value.flipX;
		ImGui::TextUnformatted("Flip X");
		ImGui::SameLine();
		if (ImGui::Checkbox("##FlipX", &flipX))
		{
			if (tileIsSelectedNow)
				ApplyToSelectedTiles([&](TileDrawData& tile) { tile.flipX = flipX; });
			else
				value.flipX = flipX;

			changed = true;
		}

		bool flipY = value.flipY;
		ImGui::TextUnformatted("Flip Y");
		ImGui::SameLine();
		if (ImGui::Checkbox("##FlipY", &flipY))
		{
			if (tileIsSelectedNow)
				ApplyToSelectedTiles([&](TileDrawData& tile) { tile.flipY = flipY; });
			else
				value.flipY = flipY;

			changed = true;
		}

		ImGui::TreePop();
	}

	ImGui::PopID();
	return changed;
}

bool buki::InspectorPropertyDrawer::DrawTileListProperty(const PropertyInfo& propInfo, void* fieldPtr)
{
	std::vector<TileDrawData>* value = reinterpret_cast<std::vector<TileDrawData>*>(fieldPtr);
	if (value == nullptr)
		return false;

	if (propInfo.prop.fields.empty())
		return false;

	const BukiField& lastField = propInfo.prop.fields.back();
	bool changed = false;

	ImGui::PushID(lastField.name.c_str());

	if (ImGui::TreeNode(lastField.key.c_str()))
	{
		const ImGuiID selectionId = ImGui::GetID("TileSelectionSet");
		std::vector<size_t>& selectedTiles = sTileSelections[selectionId];

		const ImGuiID rectPickerId = ImGui::GetID("SharedTileRectPicker");
		TileRectSelectorState& rectState = sTileRectPickerStates[rectPickerId];

		bool openRectWindowRequested = false;
		ImVec2 rectWindowAnchor(0.0f, 0.0f);

		for (size_t i = 0; i < value->size(); ++i)
		{
			changed |= DrawTileProperty(
				propInfo,
				*value,
				i,
				selectedTiles,
				openRectWindowRequested,
				rectWindowAnchor);
		}

		selectedTiles.erase(
			std::remove_if(
				selectedTiles.begin(),
				selectedTiles.end(),
				[&](size_t i) { return i >= value->size(); }),
			selectedTiles.end());

		if (selectedTiles.empty() && !value->empty())
			selectedTiles.push_back(0);

		if (openRectWindowRequested)
		{
			rectState.open = true;
			rectState.setInitialPos = true;
			rectState.initialPos = rectWindowAnchor;
		}

		if (rectState.open && !selectedTiles.empty())
		{
			const size_t primaryIndex = selectedTiles.front();

			buki::RectF sharedRect = (*value)[primaryIndex].sourceRectPixels;
			std::string sharedImagePath = (*value)[primaryIndex].texturePath;

			if (DrawTileSourceRectWindow(
				"Tile Source Rect##SharedTileRectPicker",
				sharedImagePath,
				sharedRect,
				rectState))
			{
				for (size_t selectedIndex : selectedTiles)
				{
					if (selectedIndex < value->size())
						(*value)[selectedIndex].sourceRectPixels = sharedRect;
				}

				changed = true;
			}
		}

		ImGui::TreePop();
	}

	ImGui::PopID();
	return changed;
}

bool buki::InspectorPropertyDrawer::DrawSharedComponent(const std::string& componentTypeName, const std::vector<Entity*>& entities)
{
	if (entities.empty())
		return false;

	for (Entity* entity : entities)
	{
		if (entity == nullptr)
			return false;
	}

	Component* first = entities[0]->GetComponentByTypeName(componentTypeName);
	if (first == nullptr)
		return false;

	bool changed = false;
	std::string currentGroup;

	for (const PropertyInfo& propInfo : first->GetProperties())
	{
		std::vector<BukiField> fields = propInfo.prop.fields;
		BukiField lastField = fields[fields.size() - 1];
		if (!propInfo.prop.group.empty() && propInfo.prop.group != currentGroup)
		{
			currentGroup = propInfo.prop.group;
			ImGui::SeparatorText(currentGroup.c_str());
		}

		switch (propInfo.prop.type)
		{
		case PropertyType::Int:
			changed |= DrawSharedIntProperty(
				propInfo,
				GatherSharedFieldPtrs<int>(componentTypeName, entities, lastField.offset));
			break;

		case PropertyType::Float:
			changed |= DrawSharedFloatProperty(
				propInfo,
				GatherSharedFieldPtrs<float>(componentTypeName, entities, lastField.offset));
			break;

		case PropertyType::Bool:
			changed |= DrawSharedBoolProperty(
				propInfo,
				GatherSharedFieldPtrs<bool>(componentTypeName, entities, lastField.offset));
			break;

		case PropertyType::String:
			changed |= DrawSharedStringProperty(
				propInfo,
				GatherSharedFieldPtrs<std::string>(componentTypeName, entities, lastField.offset));
			break;

		case PropertyType::StringList:
			changed |= DrawSharedStringListProperty(
				propInfo,
				GatherSharedFieldPtrs<std::vector<std::string>>(componentTypeName, entities, lastField.offset));
			break;

		case PropertyType::ImageAsset:
			changed |= DrawSharedAssetProperty(
				propInfo,
				GatherSharedFieldPtrs<std::string>(componentTypeName, entities, lastField.offset),
				"IMAGE");
			break;

		case PropertyType::AudioAsset:
			changed |= DrawSharedAssetProperty(
				propInfo,
				GatherSharedFieldPtrs<std::string>(componentTypeName, entities, lastField.offset),
				"AUDIO");
			break;

		case PropertyType::FontAsset:
			changed |= DrawSharedAssetProperty(
				propInfo,
				GatherSharedFieldPtrs<std::string>(componentTypeName, entities, lastField.offset),
				"FONT");
			break;

		case PropertyType::PrefabRef:
			changed |= DrawSharedAssetProperty(
				propInfo,
				GatherSharedFieldPtrs<std::string>(componentTypeName, entities, lastField.offset),
				"PREFAB");
			break;

		case PropertyType::ImageAssetList:
			changed |= DrawSharedAssetListProperty(
				propInfo,
				GatherSharedFieldPtrs<std::vector<std::string>>(componentTypeName, entities, lastField.offset),
				"IMAGE");
			break;

		case PropertyType::AudioAssetList:
			changed |= DrawSharedAssetListProperty(
				propInfo,
				GatherSharedFieldPtrs<std::vector<std::string>>(componentTypeName, entities, lastField.offset),
				"AUDIO");
			break;

		case PropertyType::FontAssetList:
			changed |= DrawSharedAssetListProperty(
				propInfo,
				GatherSharedFieldPtrs<std::vector<std::string>>(componentTypeName, entities, lastField.offset),
				"FONT");
			break;

		case PropertyType::EntityRef:
			changed |= DrawSharedEntityRefProperty(
				propInfo,
				GatherSharedFieldPtrs<EntityRef>(componentTypeName, entities, lastField.offset));
			break;

		case PropertyType::Vector2:
			changed |= DrawSharedVector2Property(
				propInfo,
				GatherSharedFieldPtrs<Vector2>(componentTypeName, entities, lastField.offset));
			break;

		case PropertyType::Color:
			changed |= DrawSharedColorProperty(
				propInfo,
				GatherSharedFieldPtrs<Color>(componentTypeName, entities, lastField.offset));
			break;

		case PropertyType::RectF:
			changed |= DrawSharedRectFProperty(
				propInfo,
				GatherSharedFieldPtrs<RectF>(componentTypeName, entities, lastField.offset));
			break;

		case PropertyType::EnumInt:
			changed |= DrawSharedEnumIntProperty(
				propInfo,
				GatherSharedFieldPtrs<int>(componentTypeName, entities, lastField.offset));
			break;

		default:
			ImGui::TextDisabled("%s: unsupported property type", lastField.key.c_str());
			break;
		}
	}

	return changed;
}

bool buki::InspectorPropertyDrawer::DrawCheckboxMixedFallback(const char* label, const char* hiddenId, bool& value, bool mixed)
{
	if (mixed)
	{
		ImGui::TextUnformatted(label);
		ImGui::SameLine();
		ImGui::TextDisabled("(mixed)");
		return ImGui::Checkbox(hiddenId, &value);
	}

	return ImGui::Checkbox(label, &value);
}

bool buki::InspectorPropertyDrawer::DrawAddComponentPopup(Entity* owner)
{
	if (owner == nullptr)
		return false;

	bool changed = false;

	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponentPopup");

	if (ImGui::BeginPopup("AddComponentPopup"))
	{
		if (ImGui::BeginMenu("Registered Components"))
		{
			for (const std::string& typeName : ComponentFactory::GetRegisteredTypeNames())
			{
				if (ImGui::MenuItem(typeName.c_str()))
				{
					if (!owner->HasComponent(typeName))
					{
						owner->AddComponentByTypeName(typeName);
						changed = true;
					}
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}

	return changed;
}

bool buki::InspectorPropertyDrawer::DrawAddComponentPopup(const std::vector<Entity*>& owners)
{
	if (owners.empty())
		return false;

	bool changed = false;

	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddSharedComponentPopup");

	if (ImGui::BeginPopup("AddSharedComponentPopup"))
	{
		if (ImGui::BeginMenu("Registered Components"))
		{
			for (const std::string& typeName : ComponentFactory::GetRegisteredTypeNames())
			{
				if (ImGui::MenuItem(typeName.c_str()))
				{
					for (Entity* owner : owners)
					{
						if (owner != nullptr && !owner->HasComponent(typeName))
						{
							owner->AddComponentByTypeName(typeName);
							changed = true;
						}
					}
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}

	return changed;
}

std::vector<std::string> buki::InspectorPropertyDrawer::GetSharedComponentTypeNames(const std::vector<Entity*>& entities)
{
	std::vector<std::string> result;

	if (entities.empty() || entities[0] == nullptr)
		return result;

	for (auto& [type, component] : entities[0]->GetComponents())
	{
		if (component == nullptr)
			continue;

		std::string typeName = ComponentFactory::GetTypeName(*type);
		if (typeName.empty())
			typeName = type->name();

		bool existsOnAll = true;
		for (size_t i = 1; i < entities.size(); ++i)
		{
			Entity* entity = entities[i];
			if (entity == nullptr || !entity->HasComponent(typeName))
			{
				existsOnAll = false;
				break;
			}
		}

		if (existsOnAll)
			result.push_back(typeName);
	}

	return result;
}
