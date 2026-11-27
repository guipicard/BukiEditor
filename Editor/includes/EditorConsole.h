#pragma once

#include <deque>
#include <mutex>
#include <string>

namespace buki
{
    enum class ConsoleLevel
    {
        Info,
        Warning,
        Error
    };

    struct ConsoleLine
    {
        ConsoleLevel level = ConsoleLevel::Info;
        std::string source;
        std::string text;
    };

    class EditorConsole
    {
    public:
        void Add(ConsoleLevel level, const std::string& source, const std::string& text);
        void AddClassified(const std::string& source, const std::string& text);
        void Clear();
        void Draw(bool* open);

    private:
        static ConsoleLevel Classify(const std::string& text);

        mutable std::mutex m_Mutex;
        std::deque<ConsoleLine> m_Lines;
        char m_Filter[128] = "";
        bool m_AutoScroll = true;
        bool m_ShowInfo = true;
        bool m_ShowWarnings = true;
        bool m_ShowErrors = true;

        static constexpr size_t MaxLines = 5000;
    };
}