#pragma once

#include <string>

#include "ClipAsset.h"
#include "imgui.h"

namespace buki
{
    class ClipAssetEditorWindow
    {
    public:
        bool Open(const std::string& path);
        bool IsOpenForPath(const std::string& path) const;
        void RequestFocus();
        bool Save();

        void Draw();

        bool IsOpen() const;
        void Close();

        bool ShouldDockNextToScene() const;

    private:
        void DrawAssetSection();
        void DrawFramesSection();

        ClipAsset asset;
        std::string assetPath;

        bool open = false;
        bool selected = false;
        bool requestDockNextToScene = false;
        bool requestFocus = false;
    };
}