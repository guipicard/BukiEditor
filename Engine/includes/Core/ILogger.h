#pragma once

#include <string_view>

namespace buki
{
    enum class LogLevel
    {
        Message,
        Success,
        Warning,
        Error
    };

    struct ILogger
    {
        virtual ~ILogger() = default;

        virtual void Log(LogLevel level, std::string_view text) = 0;

        virtual void LogMessage(std::string_view text) = 0;
        virtual void LogSuccess(std::string_view text) = 0;
        virtual void LogWarning(std::string_view text) = 0;
        virtual void LogError(std::string_view text) = 0;
        virtual void LogSdlError() = 0;
    };
}
