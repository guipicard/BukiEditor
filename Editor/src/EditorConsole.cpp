#include "EditorConsole.h"

#include "imgui.h"

#include <algorithm>
#include <cctype>

namespace buki
{
    void EditorConsole::Add(ConsoleLevel level, const std::string& source, const std::string& text)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);

        m_Lines.push_back({ level, source, text });

        while (m_Lines.size() > MaxLines)
        {
            m_Lines.pop_front();
        }
    }

    void EditorConsole::AddClassified(const std::string& source, const std::string& text)
    {
        Add(Classify(text), source, text);
    }

    void EditorConsole::Clear()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Lines.clear();
    }

    ConsoleLevel EditorConsole::Classify(const std::string& text)
    {
        std::string lower = text;
        std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        const char* errorMarkers[] = { ": error", "fatal error", "error c", "error lnk", "[error", "[err]", "error:", "exception", "access violation" };
        const char* warningMarkers[] = { ": warning", "warning c", "[warn", "warning:" };

        for (const char* marker : errorMarkers)
        {
            if (lower.find(marker) != std::string::npos)
            {
                return ConsoleLevel::Error;
            }
        }

        for (const char* marker : warningMarkers)
        {
            if (lower.find(marker) != std::string::npos)
            {
                return ConsoleLevel::Warning;
            }
        }

        return ConsoleLevel::Info;
    }

    void EditorConsole::Draw(bool* open)
    {
        if (!ImGui::Begin("Console", open))
        {
            ImGui::End();
            return;
        }

        if (ImGui::Button("Clear"))
        {
            Clear();
        }

        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &m_AutoScroll);
        ImGui::SameLine();
        ImGui::Checkbox("Info", &m_ShowInfo);
        ImGui::SameLine();
        ImGui::Checkbox("Warnings", &m_ShowWarnings);
        ImGui::SameLine();
        ImGui::Checkbox("Errors", &m_ShowErrors);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(180.0f);
        ImGui::InputTextWithHint("##consoleFilter", "Filter...", m_Filter, sizeof(m_Filter));
        ImGui::Separator();

        if (ImGui::BeginChild("##consoleLines", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_HorizontalScrollbar))
        {
            std::string filter = m_Filter;
            std::transform(filter.begin(), filter.end(), filter.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

            std::lock_guard<std::mutex> lock(m_Mutex);

            int index = 0;

            for (const ConsoleLine& line : m_Lines)
            {
                ++index;

                if ((line.level == ConsoleLevel::Info && !m_ShowInfo) || (line.level == ConsoleLevel::Warning && !m_ShowWarnings) || (line.level == ConsoleLevel::Error && !m_ShowErrors))
                {
                    continue;
                }

                if (!filter.empty())
                {
                    std::string lower = line.text;
                    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

                    if (lower.find(filter) == std::string::npos)
                    {
                        continue;
                    }
                }

                ImVec4 color(0.85f, 0.85f, 0.85f, 1.0f);

                if (line.level == ConsoleLevel::Warning)
                {
                    color = ImVec4(1.0f, 0.8f, 0.3f, 1.0f);
                }
                else if (line.level == ConsoleLevel::Error)
                {
                    color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                }

                ImGui::PushID(index);
                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::Text("[%s] %s", line.source.c_str(), line.text.c_str());
                ImGui::PopStyleColor();

                if (ImGui::BeginPopupContextItem("##consoleLineMenu"))
                {
                    if (ImGui::MenuItem("Copy line"))
                    {
                        ImGui::SetClipboardText(line.text.c_str());
                    }

                    ImGui::EndPopup();
                }

                ImGui::PopID();
            }

            if (m_AutoScroll)
            {
                ImGui::SetScrollHereY(1.0f);
            }
        }

        ImGui::EndChild();
        ImGui::End();
    }
}