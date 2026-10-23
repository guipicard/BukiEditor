#pragma once
#include "AnimationassetEditor.h"
#include "InspectorPropertyDrawer.h"
#include "imgui.h"
#include "PropertyInfo.h"

bool buki::AnimationAssetEditor::Load(const std::string& path)
{
	buki::AnimationAsset loaded;
	if (!loaded.LoadFromFile(path))
		return false;

	assetPath = path;
	asset = std::move(loaded);
	open = true;
	return true;
}

bool buki::AnimationAssetEditor::Save()
{
	return asset.SaveToFile(assetPath);
}

void buki::AnimationAssetEditor::DrawInspector()
{
    if (!open)
        return;

    char nameBuffer[256];
    std::snprintf(nameBuffer, sizeof(nameBuffer), "%s", asset.name.c_str());

    if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
        asset.name = nameBuffer;

    char initialStateBuffer[256];
    std::snprintf(initialStateBuffer, sizeof(initialStateBuffer), "%s", asset.initialState.c_str());

    if (ImGui::InputText("Initial State", initialStateBuffer, sizeof(initialStateBuffer)))
        asset.initialState = initialStateBuffer;

    ImGui::Separator();

    if (ImGui::Button("Add State"))
    {
        AnimationState state;
        state.name = "NewState";
        state.clipName = "";
        asset.states.push_back(state);
    }

    int stateToRemove = -1;

    for (int stateIndex = 0; stateIndex < static_cast<int>(asset.states.size()); ++stateIndex)
    {
        AnimationState& state = asset.states[stateIndex];

        ImGui::PushID(stateIndex);

        if (ImGui::TreeNode(("state" + std::to_string(stateIndex)).c_str()))
        {
            char stateNameBuffer[256];
            std::snprintf(stateNameBuffer, sizeof(stateNameBuffer), "%s", state.name.c_str());

            if (ImGui::InputText("Name", stateNameBuffer, sizeof(stateNameBuffer)))
                state.name = stateNameBuffer;

            char clipNameBuffer[256];
            std::snprintf(clipNameBuffer, sizeof(clipNameBuffer), "%s", state.clipName.c_str());

            if (ImGui::InputText("Clip", clipNameBuffer, sizeof(clipNameBuffer)))
                state.clipName = clipNameBuffer;

            if (ImGui::Button("Remove State"))
                stateToRemove = stateIndex;

            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    if (stateToRemove >= 0)
        asset.states.erase(asset.states.begin() + stateToRemove);

    ImGui::Separator();

    if (ImGui::Button("Add Clip"))
    {
        AnimationClip clip;
        clip.name = "NewClip";
        asset.clips.push_back(clip);
    }

    for (size_t i = 0; i < asset.clips.size(); i++)
    {
        auto& clip = asset.clips[i];
        ImGui::PushID(("clip" + std::to_string(i)).c_str());

        if (ImGui::TreeNode(("clip" + std::to_string(i)).c_str()))
        {
            char clipNameBuffer[256];
            std::snprintf(clipNameBuffer, sizeof(clipNameBuffer), "%s", clip.name.c_str());

            if (ImGui::InputText("Clip Name", clipNameBuffer, sizeof(clipNameBuffer)))
                clip.name = clipNameBuffer;

            ImGui::Checkbox("Loop", &clip.loop);


            int frameToRemove = -1;

            for (int frameIndex = 0; frameIndex < static_cast<int>(clip.frames.size()); ++frameIndex)
            {
                SpriteFrame& frame = clip.frames[frameIndex];

                ImGui::PushID(frameIndex);
                PropertyInfo prop = BUKI_PROP_STRING(SpriteFrame, texturePath);
                InspectorPropertyDrawer::DrawStringAssetPicker(prop, &frame.texturePath, "IMAGE");
                ImGui::DragFloat("Duration", &frame.duration, 0.01f, 0.01f, 10.0f);
                ImGui::Checkbox("Flip X", &frame.flipX);
                ImGui::Checkbox("Flip Y", &frame.flipY);

                if (ImGui::Button("Remove Frame"))
                    frameToRemove = frameIndex;

                ImGui::PopID();
            }

            if (frameToRemove >= 0)
                clip.frames.erase(clip.frames.begin() + frameToRemove);

            ImGui::TreePop();
        }

            if (ImGui::Button("Add Frame"))
            {
                SpriteFrame frame;
                frame.texturePath = "";
                frame.duration = 0.1f;
                clip.frames.push_back(frame);
            }
        ImGui::PopID();
    }

    ImGui::Separator();

    if (ImGui::Button("Save"))
        Save();
}