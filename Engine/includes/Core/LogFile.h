#pragma once

#include "ILogger.h"
#include <fstream>
#include <string>

namespace buki
{
    class LogFile final : public ILogger
    {
    public:
        explicit LogFile(const std::string& path);
        ~LogFile() override;

        void Log(LogLevel level, std::string_view text) override;
        void LogMessage(std::string_view text) override;
        void LogSuccess(std::string_view text) override;
        void LogWarning(std::string_view text) override;
        void LogError(std::string_view text) override;
        void LogSdlError() override;

    private:
        std::ofstream m_File;
    };
}
