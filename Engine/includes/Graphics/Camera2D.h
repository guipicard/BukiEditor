#pragma once

#include <glm/vec2.hpp>
#include "Units.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace buki
{
    struct Camera2D
    {
        glm::vec2 position{ 0.0f, 0.0f }; // meters
        float zoom = 1.0f;

        float viewportWidth = 1280.0f;   // pixels
        float viewportHeight = 720.0f;   // pixels

        [[nodiscard]] void GetViewportWorldSize(float* width, float* height) const
        {
            if (width)
            {
                *width = Units::ToMeters(viewportWidth);
            }
            if (height)
            {
                *height = Units::ToMeters(viewportHeight);
            }
        }

        [[nodiscard]] glm::vec2 GetViewportCenter() const
        {
            return { viewportWidth * 0.5f, viewportHeight * 0.5f };
        }

        json Serialize() const
        {
			json doc;
            doc["position"]["x"] = position.x;
            doc["position"]["y"] = position.y;
            doc["zoom"] = zoom;
            doc["viewportWidth"] = viewportWidth;
            doc["viewportHeight"] = viewportHeight;
            return doc;
		}

        void Deserialize(const json& doc)
        {
            position.x = doc["position"].value("x", position.x);
            position.y = doc["position"].value("y", position.y);
            zoom = doc.value("zoom", zoom);
            viewportWidth = doc.value("viewportWidth", viewportWidth);
            viewportHeight = doc.value("viewportHeight", viewportHeight);
		}
    };
}