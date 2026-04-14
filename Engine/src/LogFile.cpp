#include "LogFile.h"

#include <SDL3/SDL_error.h>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace buki
{
    LogFile::LogFile(const std::string& path)
    {
        m_File.open(path, std::ios::out | std::ios::app);

        if (!m_File.is_open())
        {
            return;
        }

        const auto now = std::chrono::system_clock::now();
        const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);

        std::tm localTime{};
#ifdef _WIN32
        localtime_s(&localTime, &nowTime);
#else
        localtime_r(&nowTime, &localTime);
#endif

        m_File << "\n========== Log Session: "
            << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S")
            << " ==========\n";
    }

    LogFile::~LogFile()
    {
        if (m_File.is_open())
        {
            m_File << "========== End Session ==========\n";
            m_File.close();
        }
    }

    void LogFile::Log(LogLevel level, std::string_view text)
    {
        if (!m_File.is_open())
        {
            return;
        }

        switch (level)
        {
        case LogLevel::Success: m_File << "[Success] "; break;
        case LogLevel::Warning: m_File << "[Warning] "; break;
        case LogLevel::Error:   m_File << "[Error] ";   break;
        case LogLevel::Message:
        default:                break;
        }

        m_File << text << '\n';
        m_File.flush();
    }

    void LogFile::LogMessage(std::string_view text) { Log(LogLevel::Message, text); }
    void LogFile::LogSuccess(std::string_view text) { Log(LogLevel::Success, text); }
    void LogFile::LogWarning(std::string_view text) { Log(LogLevel::Warning, text); }
    void LogFile::LogError(std::string_view text) { Log(LogLevel::Error, text); }
    void LogFile::LogSdlError() { LogError(SDL_GetError()); }
}
