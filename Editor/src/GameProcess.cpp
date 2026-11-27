#include "GameProcess.h"

#include <algorithm>
#include <vector>

namespace buki
{
    namespace
    {
        std::string OemToUtf8(const std::string& text)
        {
            if (text.empty())
            {
                return text;
            }

            const int wideLength = MultiByteToWideChar(CP_OEMCP, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);

            if (wideLength <= 0)
            {
                return text;
            }

            std::wstring wide(static_cast<size_t>(wideLength), L'\0');
            MultiByteToWideChar(CP_OEMCP, 0, text.data(), static_cast<int>(text.size()), wide.data(), wideLength);

            const int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wide.data(), wideLength, nullptr, 0, nullptr, nullptr);

            if (utf8Length <= 0)
            {
                return text;
            }

            std::string utf8(static_cast<size_t>(utf8Length), '\0');
            WideCharToMultiByte(CP_UTF8, 0, wide.data(), wideLength, utf8.data(), utf8Length, nullptr, nullptr);
            return utf8;
        }
    }

    GameProcess::~GameProcess()
    {
        Stop();

        if (m_Job != nullptr)
        {
            CloseHandle(m_Job);
            m_Job = nullptr;
        }
    }

    bool GameProcess::Launch(const std::string& commandLine, const std::string& workingDirectory, LineCallback onLine, bool oemOutput)
    {
        Stop();

        if (m_Job == nullptr)
        {
            m_Job = CreateJobObjectA(nullptr, nullptr);

            if (m_Job != nullptr)
            {
                JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};
                limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
                SetInformationJobObject(m_Job, JobObjectExtendedLimitInformation, &limits, sizeof(limits));
            }
        }

        const bool capture = static_cast<bool>(onLine);

        SECURITY_ATTRIBUTES inheritable{};
        inheritable.nLength = sizeof(inheritable);
        inheritable.bInheritHandle = TRUE;

        HANDLE readPipe = nullptr;
        HANDLE writePipe = nullptr;
        HANDLE nullInput = nullptr;

        STARTUPINFOA startup{};
        startup.cb = sizeof(startup);

        if (capture)
        {
            if (!CreatePipe(&readPipe, &writePipe, &inheritable, 0))
            {
                m_LastError = "CreatePipe failed (" + std::to_string(GetLastError()) + ")";
                return false;
            }

            SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);

            nullInput = CreateFileA("NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &inheritable, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

            if (nullInput == INVALID_HANDLE_VALUE)
            {
                nullInput = nullptr;
            }

            startup.dwFlags = STARTF_USESTDHANDLES;
            startup.hStdInput = nullInput;
            startup.hStdOutput = writePipe;
            startup.hStdError = writePipe;
        }

        std::vector<char> mutableCommand(commandLine.begin(), commandLine.end());
        mutableCommand.push_back('\0');

        m_ProcessInfo = {};

        const BOOL created = CreateProcessA(nullptr, mutableCommand.data(), nullptr, nullptr, capture ? TRUE : FALSE, CREATE_NO_WINDOW, nullptr, workingDirectory.c_str(), &startup, &m_ProcessInfo);

        if (writePipe != nullptr)
        {
            CloseHandle(writePipe);
        }

        if (nullInput != nullptr)
        {
            CloseHandle(nullInput);
        }

        if (!created)
        {
            m_LastError = "CreateProcess failed (" + std::to_string(GetLastError()) + ")";

            if (readPipe != nullptr)
            {
                CloseHandle(readPipe);
            }

            return false;
        }

        if (m_Job != nullptr)
        {
            AssignProcessToJobObject(m_Job, m_ProcessInfo.hProcess);
        }

        if (capture)
        {
            m_Reader = std::thread(&GameProcess::ReadLoop, readPipe, m_ProcessInfo.hProcess, std::move(onLine), oemOutput);
        }

        m_Running = true;
        m_ExitCode = 0;
        m_LastError.clear();
        return true;
    }

    void GameProcess::ReadLoop(HANDLE readPipe, HANDLE process, LineCallback onLine, bool oemOutput)
    {
        std::string pending;
        char buffer[4096];

        const auto emit = [&](std::string line)
            {
                if (!line.empty() && line.back() == '\r')
                {
                    line.pop_back();
                }

                onLine(oemOutput ? OemToUtf8(line) : line);
            };

        const auto drain = [&]() -> bool
            {
                DWORD available = 0;

                if (!PeekNamedPipe(readPipe, nullptr, 0, nullptr, &available, nullptr) || available == 0)
                {
                    return false;
                }

                DWORD read = 0;

                if (!ReadFile(readPipe, buffer, (std::min)(available, static_cast<DWORD>(sizeof(buffer))), &read, nullptr) || read == 0)
                {
                    return false;
                }

                pending.append(buffer, read);

                size_t newline = 0;

                while ((newline = pending.find('\n')) != std::string::npos)
                {
                    emit(pending.substr(0, newline));
                    pending.erase(0, newline + 1);
                }

                return true;
            };

        for (;;)
        {
            if (drain())
            {
                continue;
            }

            if (WaitForSingleObject(process, 0) == WAIT_OBJECT_0)
            {
                while (drain())
                {
                }

                break;
            }

            Sleep(10);
        }

        if (!pending.empty())
        {
            emit(pending);
        }

        CloseHandle(readPipe);
    }

    void GameProcess::Finish()
    {
        if (m_Reader.joinable())
        {
            m_Reader.join();
        }

        DWORD code = 0;

        if (m_ProcessInfo.hProcess != nullptr && GetExitCodeProcess(m_ProcessInfo.hProcess, &code))
        {
            m_ExitCode = code;
        }

        CloseHandles();
        m_Running = false;
    }

    void GameProcess::Stop()
    {
        if (!m_Running)
        {
            return;
        }

        if (m_Job != nullptr)
        {
            TerminateJobObject(m_Job, 0);
        }
        else
        {
            TerminateProcess(m_ProcessInfo.hProcess, 0);
        }

        WaitForSingleObject(m_ProcessInfo.hProcess, 2000);
        Finish();
    }

    void GameProcess::Poll()
    {
        if (m_Running && WaitForSingleObject(m_ProcessInfo.hProcess, 0) == WAIT_OBJECT_0)
        {
            Finish();
        }
    }

    void GameProcess::CloseHandles()
    {
        if (m_ProcessInfo.hThread != nullptr)
        {
            CloseHandle(m_ProcessInfo.hThread);
            m_ProcessInfo.hThread = nullptr;
        }

        if (m_ProcessInfo.hProcess != nullptr)
        {
            CloseHandle(m_ProcessInfo.hProcess);
            m_ProcessInfo.hProcess = nullptr;
        }
    }
}