#pragma once

#include "BukiContainers.h"
#include "Camera2D.h"
#include "Engine.h"

#include <string>
#include <vector>
#include <fstream>

#include <filesystem>
#include "nlohmann/json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace buki
{
    class Entity;

    enum class ViewportDisplayMode
    {
        Stretch = 0,
        Fit = 1,
        OneToOne = 2
    };

    struct CameraSettings
    {
        Vector2 position = { 0.0f, 0.0f };
        float zoom = 1.0f;
        float viewportWidth = 0.0f;
        float viewportHeight = 0.0f;
        ViewportDisplayMode displayMode = ViewportDisplayMode::Fit;

        void Serialize(const fs::path& path)
        {
            const fs::path editorSettingsPath =
                fs::absolute(
                    "../Deployment/Editor.json");

            const std::string fileName = path.stem().string();
            const std::string extension = path.extension().string();

            json document = json::object();

            std::ifstream inputFile(editorSettingsPath);

            if (inputFile.is_open())
            {
                try
                {
                    inputFile >> document;
                }
                catch (...)
                {
                    document = json::object();
                }
            }

            const std::string key = path.string();

            const bool firstPass = !document.contains(key);

            if (firstPass)
            {
                if (extension == ".scene")
                {
                    json sceneDocument = json::object();

                    std::ifstream sceneFile(
                        fs::absolute(
                            "../Deployment/" + path.string()));

                    if (sceneFile.is_open())
                    {
                        try
                        {
                            sceneFile >> sceneDocument;
                        }
                        catch (...)
                        {
                            sceneDocument = json::object();
                        }
                    }

                    document[key]["position"]["x"] =
                        sceneDocument["camera"]["position"]["x"];

                    document[key]["position"]["y"] =
                        sceneDocument["camera"]["position"]["y"];
                }
                else if (extension == ".prefab")
                {
                    json prefabDocument = json::object();

                    std::ifstream prefabFile(
                        fs::absolute(
                            "../Deployment/" + path.string()));

                    if (prefabFile.is_open())
                    {
                        try
                        {
                            prefabFile >> prefabDocument;
                        }
                        catch (...)
                        {
                            prefabDocument = json::object();
                        }
                    }

                    document[key]["position"]["x"] =
                        prefabDocument["position"]["x"];

                    document[key]["position"]["y"] =
                        prefabDocument["position"]["y"];
                }
            }
            else
            {
                document[key]["position"]["x"] = position.x;
                document[key]["position"]["y"] = position.y;
            }

            document[key]["zoom"] = zoom;
            document[key]["viewportWidth"] = viewportWidth;
            document[key]["viewportHeight"] = viewportHeight;
            document[key]["displayMode"] =
                static_cast<int>(displayMode);

            document[key]["type"] = extension;
            document[key]["path"] = key;

            std::ofstream outputFile(
                editorSettingsPath,
                std::ios::out | std::ios::trunc);

            if (outputFile.is_open())
            {
                outputFile << document.dump(4);
            }

            if (firstPass)
            {
                Deserialize(path);
            }
        }

        void Deserialize(const fs::path& path)
        {
            const fs::path editorSettingsPath =
                fs::absolute(
                    "../Deployment/Editor.json");

            json document = json::object();

            std::ifstream inputFile(editorSettingsPath);

            if (!inputFile.is_open())
            {
                return;
            }

            try
            {
                inputFile >> document;
            }
            catch (...)
            {
                return;
            }

            const std::string key = path.string();

            if (!document.contains(key))
            {
                return;
            }

            const json& node = document[key];

            position.x = node["position"].value("x", 0.0f);
            position.y = node["position"].value("y", 0.0f);

            zoom = node.value("zoom", 1.0f);

            viewportWidth =
                node.value("viewportWidth", 0.0f);

            viewportHeight =
                node.value("viewportHeight", 0.0f);

            displayMode = static_cast<ViewportDisplayMode>(
                node.value(
                    "displayMode",
                    static_cast<int>(
                        ViewportDisplayMode::Fit)));

            Camera2D* camera = Engine::Get().GetActiveCameraPtr();

            if (camera)
            {
                camera->position =
                {
                    position.x,
                    position.y
                };

                camera->zoom = zoom;
            }
        }
    };

    struct PrefabPreviewSession
    {
        std::string path;
        Entity* prefabEntity = nullptr;

        bool open = true;
        bool focused = false;
        bool hovered = false;
        bool requestFocus = false;
        bool requestDockNextToScene = false;

        std::string windowId;

        unsigned int framebuffer = 0;
        unsigned int colorTexture = 0;
        unsigned int depthRenderbuffer = 0;

        int framebufferWidth = 0;
        int framebufferHeight = 0;

        CameraSettings cameraSettings;
    };

    struct ScenePreviewSession
    {
        fs::path path;
        std::vector<Entity*> prefabEntities;

        bool open = true;
        bool focused = false;
        bool hovered = false;

        std::string windowId;

        unsigned int framebuffer = 0;
        unsigned int colorTexture = 0;
        unsigned int depthRenderbuffer = 0;

        int framebufferWidth = 0;
        int framebufferHeight = 0;

        CameraSettings cameraSettings;
    };
}