#pragma once

#include "ILogger.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace buki
{
    class LogConsole final : public ILogger
    {
    public:
        LogConsole();
        ~LogConsole() override;

        void Log(LogLevel level, std::string_view text) override;
        void LogMessage(std::string_view text) override;
        void LogSuccess(std::string_view text) override;
        void LogWarning(std::string_view text) override;
        void LogError(std::string_view text) override;
        void LogSdlError() override;

    private:
#ifdef _WIN32
        HANDLE m_ConsoleHandle = nullptr;
        WORD m_DefaultAttributes = 0;
#endif
    };
}
