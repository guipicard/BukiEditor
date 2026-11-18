#pragma once


#include <memory>
#include <string>
#include <vector>

#include "BukiContainers.h"
#include "Engine.h"

#include "AnimationAssetEditor.h"
#include "AssetBehaviorRegistry.h"
#include "AssetEditorWindowManager.h"
#include "ClipAssetEditorWindow.h"
#include "EditorAssetDatabase.h"
#include "EditorPreviewSessions.h"

namespace buki
{
    class Entity;
    class Component;

    struct EditorState
    {
        // ------------------------------------------------------------------
        // Core editor services
        // ------------------------------------------------------------------

        EditorAssetDatabase assetDatabase;
        AssetBehaviorRegistry assetBehaviors;

        AssetEditorWindowManager assetEditorWindows;

        // ------------------------------------------------------------------
        // Asset rename / delete
        // ------------------------------------------------------------------

        std::filesystem::path renamePath;
        std::string renameBuffer;
        bool renamePopupOpen = false;

        std::filesystem::path deleteCandidate;
        bool deletePopupOpen = false;

        // ------------------------------------------------------------------
        // Selection
        // ------------------------------------------------------------------

        Entity* selectedEntity = nullptr;
        std::vector<Entity*> selectedEntities;
        Entity* activeEntity = nullptr;

        Component* selectedComponent = nullptr;
        std::vector<Component*> selectedComponents;
        Component* activeComponent = nullptr;

        std::string selectedPrefabPath;
        Entity* selectedPrefabEntity = nullptr;

        std::string selectedItem;

        // ------------------------------------------------------------------
        // Editor windows
        // ------------------------------------------------------------------

        std::unique_ptr<AnimationAssetEditor> animationAssetEditor = std::make_unique<AnimationAssetEditor>();

        bool showHierarchy = true;
        bool showInspector = true;
        bool showSceneView = true;
        bool showContentBrowser = true;
        bool showDemoWindow = false;

        bool requestSceneWindowFocus = false;

        // ------------------------------------------------------------------
        // Scene viewport
        // ------------------------------------------------------------------

        float viewportWidth = 0.0f;
        float viewportHeight = 0.0f;

        unsigned int sceneFramebuffer = 0;
        unsigned int sceneColorTexture = 0;
        unsigned int sceneDepthRenderbuffer = 0;

        int sceneFramebufferWidth = 0;
        int sceneFramebufferHeight = 0;

        ViewportDisplayMode viewportDisplayMode =
            ViewportDisplayMode::OneToOne;

        bool lockViewportRenderSize = true;
        bool canSeeViewportRenderSize = true;
        bool canSeeViewportRenderInfo = false;

        int lockedViewportWidth = 1920;
        int lockedViewportHeight = 1080;

        // ------------------------------------------------------------------
        // Content browser
        // ------------------------------------------------------------------

        std::filesystem::path currentContentPath = "../Deployment";
        std::filesystem::path scenesRootPath = "../Deployment/Scenes";
        std::filesystem::path selectedScenePath;

        bool setTilesDefaults = true;

        // ------------------------------------------------------------------
        // Preview sessions
        // ------------------------------------------------------------------

        std::vector<PrefabPreviewSession> prefabPreviewSessions;
        int activePrefabPreviewIndex = -1;

        ScenePreviewSession scenePreviewSession;

        // ------------------------------------------------------------------
        // Dirty state
        // ------------------------------------------------------------------

        void MarkSceneDirty()
        {
            m_SceneDirty = true;
        }

        void ClearSceneDirty()
        {
            if (m_SceneDirty)
            {
                Engine::Get().Log().LogMessage(
                    "Scene marked as clean.");
            }

            m_SceneDirty = false;
        }

        [[nodiscard]] bool IsSceneDirty() const
        {
            return m_SceneDirty;
        }

    private:
        bool m_SceneDirty = false;
    };
}