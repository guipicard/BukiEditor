#include "ClipAssetEditorWindow.h"

#include "InspectorPropertyDrawer.h"
#include "PropertyInfo.h"
#include "imgui.h"

bool buki::ClipAssetEditorWindow::Open(const std::string& path)
{
    ClipAsset loaded;

    if (!loaded.LoadFromFile(path))
        return false;

    assetPath = path;
    asset = std::move(loaded);
    selected = true;
    open = true;
    requestDockNextToScene = true;

    return true;
}

bool buki::ClipAssetEditorWindow::Save()
{
    return asset.SaveToFile(assetPath);
}

bool buki::ClipAssetEditorWindow::IsOpenForPath(const std::string& path) const
{
    return open && assetPath == path;
}

void buki::ClipAssetEditorWindow::RequestFocus()
{
    requestFocus = true;
}

void buki::ClipAssetEditorWindow::Draw()
{
    if (!open)
        return;

    ImGui::SetNextWindowSize(
        ImVec2(700.0f, 500.0f),
        ImGuiCond_FirstUseEver);
    if (requestFocus)
    {
        ImGui::SetNextWindowFocus();
        requestFocus = false;
    }

    if (!ImGui::Begin("Animation Clip", &open))
    {
        ImGui::End();
        return;
    }

    if (!selected)
    {
        ImGui::TextDisabled("No clip loaded.");
        ImGui::End();
        return;
    }

    DrawAssetSection();
    ImGui::Separator();

    DrawFramesSection();
    ImGui::Separator();

    if (ImGui::Button("Save"))
        Save();

    ImGui::End();

    requestDockNextToScene = false;
}

void buki::ClipAssetEditorWindow::DrawAssetSection()
{
    ImGui::SeparatorText("Animation Clip");

    char nameBuffer[256];
    std::snprintf(
        nameBuffer,
        sizeof(nameBuffer),
        "%s",
        asset.name.c_str());

    if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
        asset.name = nameBuffer;

    ImGui::Checkbox("Loop", &asset.loop);
}

void buki::ClipAssetEditorWindow::DrawFramesSection()
{
    ImGui::SeparatorText("Frames");

    if (asset.frames.empty())
    {
        ImGui::TextDisabled("No frames. Add one to build the animation.");
    }

    if (ImGui::Button("Add Frame"))
    {
        SpriteFrame frame;
        frame.texturePath = "";
        frame.duration = 0.1f;

        asset.frames.push_back(frame);
    }

    int frameToRemove = -1;

    for (int frameIndex = 0;
        frameIndex < static_cast<int>(asset.frames.size());
        ++frameIndex)
    {
        SpriteFrame& frame = asset.frames[frameIndex];

        ImGui::PushID(frameIndex);

        const std::string treeNodeId =
            "Frame##" + std::to_string(frameIndex);

        if (ImGui::TreeNode(treeNodeId.c_str()))
        {
            PropertyInfo textureProperty =
                BUKI_PROP_STRING(SpriteFrame, texturePath);

            InspectorPropertyDrawer::DrawStringAssetPicker(
                textureProperty,
                &frame.texturePath,
                "IMAGE"
            );

            ImGui::DragFloat(
                "Duration",
                &frame.duration,
                0.01f,
                0.01f,
                10.0f
            );

            ImGui::Checkbox("Flip X", &frame.flipX);
            ImGui::Checkbox("Flip Y", &frame.flipY);

            if (ImGui::Button("Remove Frame"))
                frameToRemove = frameIndex;

            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    if (frameToRemove >= 0)
    {
        asset.frames.erase(
            asset.frames.begin() + frameToRemove);
    }
}

bool buki::ClipAssetEditorWindow::IsOpen() const
{
    return open;
}

void buki::ClipAssetEditorWindow::Close()
{
    open = false;
    selected = false;
    assetPath.clear();
    asset = ClipAsset{};
    requestDockNextToScene = false;
}

bool buki::ClipAssetEditorWindow::ShouldDockNextToScene() const
{
    return requestDockNextToScene;
}