#pragma once

#include <string>
#include <filesystem>

namespace buki
{
    struct EditorState;
    class AssetEditorWindow
    {
    public:
        explicit AssetEditorWindow(EditorState& editorState)
            : m_EditorState(editorState)
        {
        }

        virtual ~AssetEditorWindow() = default;

        void Open(const std::string& path)
        {
            m_AssetPath = path;

            if (!LoadAsset(path))
            {
                return;
            }

            m_Open = true;
            OnAssetLoaded();
        }

        [[nodiscard]] const std::string& GetAssetPath() const
        {
            return m_AssetPath;
        }

        [[nodiscard]] bool IsOpen() const
        {
            return m_Open;
        }

        [[nodiscard]] bool IsOpenForPath(const std::string& path) const
        {
            return m_Open && m_AssetPath == path;
        }

        [[nodiscard]] bool ShouldDockNextToScene() const
        {
            return m_RequestDockNextToScene;
        }

        [[nodiscard]] std::string GetAssetFileName() const
        {
            return std::filesystem::path(m_AssetPath).stem().string();
        }

        void Close()
        {
            m_Open = false;
            OnClosed();
        }

        void RequestFocus() 
        {
            m_RequestFocus = true;
        }

        virtual void Draw() = 0;

        virtual void Reload() = 0;

    protected:
        virtual bool LoadAsset(const std::string& path) = 0;
        virtual void OnAssetLoaded() = 0;
        virtual void OnClosed() = 0;

        EditorState& m_EditorState;
        std::string m_AssetPath;
        bool m_Open = false;
        bool m_RequestDockNextToScene = false;
        bool m_RequestFocus = false;
    };
}