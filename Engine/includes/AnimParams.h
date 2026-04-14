//#pragma once

#include <unordered_map>
#include <variant>
#include <string>

namespace buki
{
    using AnimValue = std::variant<bool, int, float, std::string>;

    class AnimationParams
    {
    public:
        AnimationParams() = default;

        // Custom params (flexible)
        std::unordered_map<std::string, AnimValue> params;

        // Safe getters with defaults
        template<typename T>
        T Get(const std::string& key, T defaultValue = T{}) const
        {
            static_assert(std::is_same_v<T, bool> || std::is_same_v<T, int> ||
                std::is_same_v<T, float> || std::is_same_v<T, std::string>,
                "Only bool, int, float, std::string supported");

            auto it = params.find(key);
            if (it == params.end())
                return defaultValue;

            if constexpr (std::is_same_v<T, bool>)
            {
                if (auto p = std::get_if<bool>(&it->second); p)
                    return *p;
            }
            else if constexpr (std::is_same_v<T, int>)
            {
                if (auto p = std::get_if<int>(&it->second); p)
                    return *p;
            }
            else if constexpr (std::is_same_v<T, float>)
            {
                if (auto p = std::get_if<float>(&it->second); p)
                    return *p;
            }
            else if constexpr (std::is_same_v<T, std::string>)
            {
                if (auto p = std::get_if<std::string>(&it->second); p)
                    return *p;
            }

            return defaultValue;
        }

        // Setters
        void Set(const std::string& key, bool value) { params[key] = value; }
        void Set(const std::string& key, int value) { params[key] = value; }
        void Set(const std::string& key, float value) { params[key] = value; }
        void Set(const std::string& key, std::string value) { params[key] = value; }
    };
}