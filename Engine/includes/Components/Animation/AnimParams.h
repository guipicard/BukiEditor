#pragma once

#include <unordered_map>
#include <variant>
#include <string>
#include <type_traits>

namespace buki
{
    using AnimValue = std::variant<bool, int, float, std::string>;

    struct AnimationParams
    {
        std::string defaultClip;
        bool playOnStart = false;
        float speed = 1.0f;
        bool loop = true;

        std::unordered_map<std::string, AnimValue> params;

        template<typename T>
        T Get(const std::string& key, T defaultValue = T{}) const
        {
            static_assert(
                std::is_same_v<T, bool> ||
                std::is_same_v<T, int> ||
                std::is_same_v<T, float> ||
                std::is_same_v<T, std::string>,
                "Only bool, int, float, std::string supported");

            if (key == "defaultClip")
            {
                if constexpr (std::is_same_v<T, std::string>)
                    return defaultClip;
            }
            else if (key == "playOnStart")
            {
                if constexpr (std::is_same_v<T, bool>)
                    return playOnStart;
            }
            else if (key == "speed")
            {
                if constexpr (std::is_same_v<T, float>)
                    return speed;
            }
            else if (key == "loop")
            {
                if constexpr (std::is_same_v<T, bool>)
                    return loop;
            }

            auto it = params.find(key);
            if (it == params.end())
                return defaultValue;

            if (auto p = std::get_if<T>(&it->second))
                return *p;

            return defaultValue;
        }

        void Set(const std::string& key, bool value)
        {
            if (key == "playOnStart")
                playOnStart = value;
            else if (key == "loop")
                loop = value;
            else
                params[key] = value;
        }

        void Set(const std::string& key, int value)
        {
            params[key] = value;
        }

        void Set(const std::string& key, float value)
        {
            if (key == "speed")
                speed = value;
            else
                params[key] = value;
        }

        void Set(const std::string& key, const std::string& value)
        {
            if (key == "defaultClip")
                defaultClip = value;
            else
                params[key] = value;
        }

        void Set(const std::string& key, std::string&& value)
        {
            if (key == "defaultClip")
                defaultClip = std::move(value);
            else
                params[key] = std::move(value);
        }
    };
}