#include "ClipAssetEditorWindow.h"

#include <algorithm>
#include <cstdio>
#include <utility>

#include "EditorAssetEntries.h"
#include "Texture2D.h"
#include "ImGui.h"
#include "InspectorPropertyDrawer.h"
#include "PropertyInfo.h"

namespace buki
{
    ClipAssetEditorWindow::ClipAssetEditorWindow(
        EditorState& editorState,
        const std::string& assetPath)
        : AssetEditorWindow(editorState)
    {
        Open(assetPath);
    }

    bool ClipAssetEditorWindow::LoadAsset(const std::string& path)
    {
        ClipAsset loaded;

        if (!loaded.LoadFromFile(path))
        {
            return false;
        }

        m_Asset = std::move(loaded);
        return true;
    }

    void ClipAssetEditorWindow::OnAssetLoaded()
    {
        m_RequestDockNextToScene = true;
        m_RequestFocus = true;
    }

    void ClipAssetEditorWindow::OnClosed()
    {
        m_Asset = ClipAsset{};

        m_RequestFocus = false;
        m_RequestDockNextToScene = false;
    }

    void ClipAssetEditorWindow::Draw()
    {
        if (!m_Open)
        {
            return;
        }

        const std::string title =
            "Clip: " +
            GetAssetFileName() +
            "###ClipEditor:" +
            m_AssetPath;

        if (m_RequestFocus)
        {
            ImGui::SetNextWindowFocus();
            m_RequestFocus = false;
        }

        bool keepOpen = true;

        const bool visible = ImGui::Begin(
            title.c_str(),
            &keepOpen);

        m_RequestDockNextToScene = false;

        if (visible && keepOpen)
        {
            if (ImGui::Button("Save"))
            {
                m_Asset.SaveToFile(m_AssetPath);
            }

            ImGui::Separator();

            char nameBuffer[512];
            std::snprintf(
                nameBuffer,
                sizeof(nameBuffer),
                "%s",
                m_Asset.name.c_str());

            if (ImGui::InputText(
                "Name",
                nameBuffer,
                sizeof(nameBuffer)))
            {
                m_Asset.name = nameBuffer;
            }

            ImGui::Checkbox("Loop", &m_Asset.loop);

            ImGui::Text(
                "Frames: %d",
                static_cast<int>(m_Asset.frames.size()));

            if (ImGui::Button("Add Frame"))
            {
                SpriteFrame frame{};
                frame.duration = 0.1f;

                m_Asset.frames.push_back(std::move(frame));
            }

            ImGui::Separator();

            int removeIndex = -1;
            int duplicateIndex = -1;
            int moveFrom = -1;
            int moveTo = -1;

            for (int index = 0;
                index < static_cast<int>(m_Asset.frames.size());
                ++index)
            {
                SpriteFrame& frame = m_Asset.frames[index];

                ImGui::PushID(index);

                const std::string frameLabel =
                    "Frame " + std::to_string(index + 1);

                const bool expanded = ImGui::TreeNodeEx(
                    frameLabel.c_str(),
                    ImGuiTreeNodeFlags_DefaultOpen);

                if (expanded)
                {
                    const Texture2D* texture = nullptr;

                    if (!frame.texturePath.empty())
                    {
                        texture = TryGetPreviewTexture(
                            frame.texturePath);
                    }

                    if (texture != nullptr &&
                        texture->IsValid() &&
                        texture->width > 0 &&
                        texture->height > 0)
                    {
                        const float textureWidth =
                            static_cast<float>(texture->width);

                        const float textureHeight =
                            static_cast<float>(texture->height);

                        ImVec2 uv0(0.0f, 0.0f);
                        ImVec2 uv1(1.0f, 1.0f);

                        float previewWidth = textureWidth;
                        float previewHeight = textureHeight;

                        if (frame.sourceRect.w > 0.0f &&
                            frame.sourceRect.h > 0.0f)
                        {
                            const float x = std::clamp(
                                frame.sourceRect.x,
                                0.0f,
                                textureWidth);

                            const float y = std::clamp(
                                frame.sourceRect.y,
                                0.0f,
                                textureHeight);

                            const float width = std::clamp(
                                frame.sourceRect.w,
                                0.0f,
                                textureWidth - x);

                            const float height = std::clamp(
                                frame.sourceRect.h,
                                0.0f,
                                textureHeight - y);

                            uv0 = ImVec2(
                                x / textureWidth,
                                y / textureHeight);

                            uv1 = ImVec2(
                                (x + width) / textureWidth,
                                (y + height) / textureHeight);

                            previewWidth = std::max(1.0f, width);
                            previewHeight = std::max(1.0f, height);
                        }

                        if (frame.flipX)
                        {
                            std::swap(uv0.x, uv1.x);
                        }

                        if (frame.flipY)
                        {
                            std::swap(uv0.y, uv1.y);
                        }

                        const float availableWidth = std::max(
                            1.0f,
                            ImGui::GetContentRegionAvail().x);

                        const float previewLimit = std::min(
                            128.0f,
                            availableWidth);

                        const float scale = previewLimit /
                            std::max(previewWidth, previewHeight);

                        ImGui::Image(
                            static_cast<ImTextureID>(texture->id),
                            ImVec2(
                                previewWidth * scale,
                                previewHeight * scale),
                            uv0,
                            uv1);
                    }
                    else
                    {
                        ImGui::TextDisabled(
                            "No valid texture selected.");
                    }
                    ImGui::TextUnformatted("Texture");

                    PropertyInfo texturePathProperty =
                        BUKI_PROP_STRING(SpriteFrame, texturePath);

                    const std::string oldTexturePath = frame.texturePath;

                    InspectorPropertyDrawer::DrawStringAssetPicker(
                        texturePathProperty,
                        &frame.texturePath,
                        "IMAGE");

                    if (frame.texturePath != oldTexturePath)
                    {
                        // The in-memory frame now references the selected texture.
                        // Live database-reference synchronization belongs here.
                    }

                    if (ImGui::DragFloat(
                        "Duration",
                        &frame.duration,
                        0.001f,
                        0.001f,
                        60.0f,
                        "%.3f s"))
                    {
                        frame.duration = std::max(
                            0.001f,
                            frame.duration);
                    }

                    float sourceRect[4] =
                    {
                        frame.sourceRect.x,
                        frame.sourceRect.y,
                        frame.sourceRect.w,
                        frame.sourceRect.h
                    };

                    if (ImGui::DragFloat4(
                        "Source Rect",
                        sourceRect,
                        1.0f))
                    {
                        frame.sourceRect.x =
                            std::max(0.0f, sourceRect[0]);

                        frame.sourceRect.y =
                            std::max(0.0f, sourceRect[1]);

                        frame.sourceRect.w =
                            std::max(0.0f, sourceRect[2]);

                        frame.sourceRect.h =
                            std::max(0.0f, sourceRect[3]);
                    }

                    ImGui::TextDisabled(
                        "Source Rect: X, Y, W, H. "
                        "Zero W or H previews the full texture.");

                    ImGui::Checkbox("Flip X", &frame.flipX);

                    ImGui::SameLine();

                    ImGui::Checkbox("Flip Y", &frame.flipY);

                    if (ImGui::Button("Duplicate"))
                    {
                        duplicateIndex = index;
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Remove"))
                    {
                        removeIndex = index;
                    }

                    if (index > 0)
                    {
                        if (ImGui::Button("Move Up"))
                        {
                            moveFrom = index;
                            moveTo = index - 1;
                        }
                    }

                    if (index + 1 <
                        static_cast<int>(m_Asset.frames.size()))
                    {
                        if (index > 0)
                        {
                            ImGui::SameLine();
                        }

                        if (ImGui::Button("Move Down"))
                        {
                            moveFrom = index;
                            moveTo = index + 1;
                        }
                    }

                    ImGui::TreePop();
                }

                ImGui::PopID();
                ImGui::Separator();
            }

            // Modify the vector only after finishing iteration.
            if (removeIndex >= 0)
            {
                m_Asset.frames.erase(
                    m_Asset.frames.begin() + removeIndex);
            }
            else if (duplicateIndex >= 0)
            {
                const SpriteFrame copy =
                    m_Asset.frames[duplicateIndex];

                m_Asset.frames.insert(
                    m_Asset.frames.begin() + duplicateIndex + 1,
                    copy);
            }
            else if (moveFrom >= 0 && moveTo >= 0)
            {
                std::swap(
                    m_Asset.frames[moveFrom],
                    m_Asset.frames[moveTo]);
            }
        }

        ImGui::End();

        if (!keepOpen)
        {
            Close();
        }
    }

    void ClipAssetEditorWindow::Reload()
    {
        // LoadAsset replaces m_Asset only after a successful load.
        // Reloading does not request docking or focus again.
        LoadAsset(m_AssetPath);
    }
}