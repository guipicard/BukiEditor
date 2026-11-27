#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <functional>
#include <string>
#include <thread>
#include <windows.h>

namespace buki
{
    class GameProcess
    {
    public:
        using LineCallback = std::function<void(const std::string&)>;

        ~GameProcess();

        bool Launch(const std::string& commandLine, const std::string& workingDirectory, LineCallback onLine = nullptr, bool oemOutput = false);
        void Stop();
        void Poll();

        bool IsRunning() const { return m_Running; }
        unsigned long ExitCode() const { return m_ExitCode; }
        const std::string& LastError() const { return m_LastError; }

    private:
        static void ReadLoop(HANDLE readPipe, HANDLE process, LineCallback onLine, bool oemOutput);
        void Finish();
        void CloseHandles();

        PROCESS_INFORMATION m_ProcessInfo{};
        HANDLE m_Job = nullptr;
        std::thread m_Reader;
        bool m_Running = false;
        unsigned long m_ExitCode = 0;
        std::string m_LastError;
    };
}