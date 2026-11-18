#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "ImGui.h"
#include "AssetEditorWindow.h"

namespace buki
{
    struct EditorState;
    class AssetEditorWindowManager
    {
    public:
        template <typename T, typename... Args>
        T* OpenAssetWindow(const std::string& assetPath, Args&&... args)
        {
            static_assert(
                std::is_base_of_v<AssetEditorWindow, T>,
                "T must inherit from AssetEditorWindow");

            if (AssetEditorWindow* existing = FindWindow(assetPath))
            {
                if (T* typedWindow = dynamic_cast<T*>(existing))
                {
                    typedWindow->RequestFocus();
                    return typedWindow;
                }

                return nullptr;
            }

            auto window = std::make_unique<T>(
                std::forward<Args>(args)...);

            T* rawWindow = window.get();

            m_Windows.push_back(std::move(window));

            return rawWindow;
        }

        [[nodiscard]] AssetEditorWindow* FindWindow(
            const std::string& assetPath)
        {
            for (const auto& window : m_Windows)
            {
                if (window->IsOpenForPath(assetPath))
                {
                    return window.get();
                }
            }

            return nullptr;
        }

        void SetAssetDockNodeId(ImGuiID dockNodeId)
        {
            m_AssetDockNodeId = dockNodeId;
        }

        [[nodiscard]] ImGuiID GetAssetDockNodeId() const
        {
            return m_AssetDockNodeId;
        }

        void DrawWindows()
        {
            for (const auto& window : m_Windows)
            {
                if (!window->IsOpen())
                {
                    continue;
                }

                if (window->ShouldDockNextToScene() &&
                    m_AssetDockNodeId != 0)
                {
                    ImGui::SetNextWindowDockID(
                        m_AssetDockNodeId,
                        ImGuiCond_Always);
                }

                window->Draw();
            }
        }

        void RemoveClosedWindows()
        {
            m_Windows.erase(
                std::remove_if(
                    m_Windows.begin(),
                    m_Windows.end(),
                    [](const std::unique_ptr<AssetEditorWindow>& window)
                    {
                        return !window->IsOpen();
                    }),
                m_Windows.end());
        }

        void CloseAll()
        {
            for (const auto& window : m_Windows)
            {
                window->Close();
            }

            m_Windows.clear();
        }

    private:
        std::vector<std::unique_ptr<AssetEditorWindow>> m_Windows;
        ImGuiID m_AssetDockNodeId = 0;
    };
}