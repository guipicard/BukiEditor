#pragma once

#include "BukiContainers.h"
#include "Camera2D.h"
#include "Engine.h"

#include <string>
#include <vector>
#include <fstream>
#include <cmath>

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

        void Serialize(const fs::path& path) const
        {
            if (path.empty())
            {
                return;
            }

            const fs::path editorSettingsPath =
                fs::absolute("../Deployment/Editor.json");

            const std::string key =
                path.lexically_normal().generic_string();

            json document = json::object();

            {
                std::ifstream input(editorSettingsPath);

                if (input.is_open())
                {
                    json parsed = json::parse(input, nullptr, false);

                    if (parsed.is_object())
                    {
                        document = std::move(parsed);
                    }
                    else
                    {
                        // Do not overwrite a malformed existing settings file.
                        return;
                    }
                }
            }

            document[key] =
            {
                {
                    "position",
                    {
                        { "x", position.x },
                        { "y", position.y }
                    }
                },
                { "zoom", zoom },
                { "viewportWidth", viewportWidth },
                { "viewportHeight", viewportHeight },
                { "displayMode", static_cast<int>(displayMode) },
                { "type", path.extension().string() },
                { "path", key }
            };

            std::ofstream output(
                editorSettingsPath,
                std::ios::out | std::ios::trunc);

            if (output.is_open())
            {
                output << document.dump(4);
            }
        }

        bool Deserialize(const fs::path& path)
        {
            if (path.empty())
            {
                return false;
            }

            const fs::path editorSettingsPath =
                fs::absolute("../Deployment/Editor.json");

            std::ifstream input(editorSettingsPath);

            if (!input.is_open())
            {
                return false;
            }

            const json document =
                json::parse(input, nullptr, false);

            if (!document.is_object())
            {
                return false;
            }

            const std::string normalizedKey =
                path.lexically_normal().generic_string();

            auto entry = document.find(normalizedKey);

            // Compatibility with keys written using path.string().
            if (entry == document.end())
            {
                entry = document.find(path.string());
            }

            if (entry == document.end() || !entry->is_object())
            {
                return false;
            }

            const json& node = *entry;

            auto readFloat = [](const json& object,
                const char* key,
                float fallback)
                {
                    const auto value = object.find(key);

                    if (value == object.end() || !value->is_number())
                    {
                        return fallback;
                    }

                    const float result = value->get<float>();

                    return std::isfinite(result) ? result : fallback;
                };

            const auto savedPosition = node.find("position");

            if (savedPosition != node.end() &&
                savedPosition->is_object())
            {
                position.x = readFloat(
                    *savedPosition, "x", position.x);

                position.y = readFloat(
                    *savedPosition, "y", position.y);
            }

            const float savedZoom =
                readFloat(node, "zoom", zoom);

            if (savedZoom > 0.0f)
            {
                zoom = savedZoom;
            }

            const auto mode = node.find("displayMode");

            if (mode != node.end() && mode->is_number_integer())
            {
                const int value = mode->get<int>();

                if (value >= static_cast<int>(ViewportDisplayMode::Stretch) &&
                    value <= static_cast<int>(ViewportDisplayMode::OneToOne))
                {
                    displayMode =
                        static_cast<ViewportDisplayMode>(value);
                }
            }

            // Viewport dimensions come from the current panel size.
            // Do not restore stale dimensions from a previous layout.

            return true;
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