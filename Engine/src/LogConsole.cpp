#include "LogConsole.h"

#include <SDL3/SDL_error.h>
#include <iostream>

namespace buki
{
    LogConsole::LogConsole()
    {
#ifdef _WIN32
        AllocConsole();
        m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (GetConsoleScreenBufferInfo(m_ConsoleHandle, &info))
        {
            m_DefaultAttributes = info.wAttributes;
        }

        FILE* dummy = nullptr;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
#endif
    }

    LogConsole::~LogConsole()
    {
#ifdef _WIN32
        FreeConsole();
#endif
    }

    void LogConsole::Log(LogLevel level, std::string_view text)
    {
#ifdef _WIN32
        WORD color = m_DefaultAttributes;

        switch (level)
        {
        case LogLevel::Success: color = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
        case LogLevel::Warning: color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
        case LogLevel::Error:   color = FOREGROUND_RED | FOREGROUND_INTENSITY; break;
        case LogLevel::Message:
        default:                color = m_DefaultAttributes; break;
        }

        SetConsoleTextAttribute(m_ConsoleHandle, color);
#endif

        switch (level)
        {
        case LogLevel::Success: std::cout << "[Success] "; break;
        case LogLevel::Warning: std::cout << "[Warning] "; break;
        case LogLevel::Error:   std::cout << "[Error] ";   break;
        case LogLevel::Message:
        default:                break;
        }

        std::cout << text << '\n';

#ifdef _WIN32
        SetConsoleTextAttribute(m_ConsoleHandle, m_DefaultAttributes);
#endif
    }

    void LogConsole::LogMessage(std::string_view text) { Log(LogLevel::Message, text); }
    void LogConsole::LogSuccess(std::string_view text) { Log(LogLevel::Success, text); }
    void LogConsole::LogWarning(std::string_view text) { Log(LogLevel::Warning, text); }
    void LogConsole::LogError(std::string_view text) { Log(LogLevel::Error, text); }
    void LogConsole::LogSdlError() { LogError(SDL_GetError()); }
}
