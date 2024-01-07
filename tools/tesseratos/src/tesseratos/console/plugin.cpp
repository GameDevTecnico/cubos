
#include <imgui.h>

#include <cubos/core/log.hpp>

#include <cubos/engine/imgui/plugin.hpp>

#include <tesseratos/console/plugin.hpp>

using cubos::engine::Cubos;

using namespace tesseratos;

std::string levelToString(cubos::core::Logger::Level level)
{
    switch (level)
    {
    case cubos::core::Logger::Level::Trace:
        return "Trace";
    case cubos::core::Logger::Level::Debug:
        return "Debug";
    case cubos::core::Logger::Level::Info:
        return "Info";
    case cubos::core::Logger::Level::Warn:
        return "Warn";
    case cubos::core::Logger::Level::Error:
        return "Error";
    case cubos::core::Logger::Level::Critical:
        return "Critical";
    case cubos::core::Logger::Level::Off:
        return "Off";
    default:
        return "Unknown";
    }
}

ImVec4 levelToColor(cubos::core::Logger::Level level)
{
    switch (level)
    {
    case cubos::core::Logger::Level::Trace:
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White
    case cubos::core::Logger::Level::Debug:
        return ImVec4(0.0f, 1.0f, 1.0f, 1.0f); // Cyan
    case cubos::core::Logger::Level::Info:
        return ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
    case cubos::core::Logger::Level::Warn:
        return ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
    case cubos::core::Logger::Level::Error:
        return ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
    case cubos::core::Logger::Level::Critical:
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White (or any color you prefer for critical)
    default:
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // Default to white
    }
}

static void console()
{
    static char searchString[256];
    std::string lastLoggedMessage = "";
    static bool collapseEnabled = false; // Flag to enable/disable collapsing of log entries

    ImGui::SetNextWindowSize(ImVec2(930, 310), ImGuiCond_FirstUseEver);
    ImGui::Begin("Console");

    ImGui::Columns(2, "ConsoleLayout", false);

    // Log Scrollable Window
    ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.8f);

    std::size_t cursor = 0;
    cubos::core::Logger::Entry entry;

    // Map to store occurrences of log entries
    std::unordered_map<std::string, int> entryOccurrences;

    ImGui::BeginChild("Log", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
    while (cubos::core::Logger::read(cursor, entry))
    {
        // Check if the log entry contains the search string
        if ((strstr(entry.message.c_str(), searchString) != nullptr))
        {
            // Check if entry is in the map
            if (entryOccurrences.find(entry.message) == entryOccurrences.end())
            {
                entryOccurrences[entry.message] = 1;
            }
            else
            {
                // Check if the current entry is consecutive to the previous one
                if (entry.message == lastLoggedMessage)
                {
                    entryOccurrences[entry.message]++;
                }
                else
                {
                    entryOccurrences[entry.message] = 1;
                }
            }

            ImVec4 textColor = levelToColor(entry.level);

            // Display the log entry
            if (!collapseEnabled || entryOccurrences[entry.message] == 1)
            {
                ImGui::TextColored(textColor, "%s: %s", levelToString(entry.level).c_str(), entry.message.c_str());
            }
            else
            {
                // Display the repetition count only when collapse is enabled and entries are consecutive
                ImGui::TextColored(textColor, "%s (%d): %s", levelToString(entry.level).c_str(),
                                   entryOccurrences[entry.message], entry.message.c_str());
            }

            // Update the last logged message for the next iteration
            lastLoggedMessage = entry.message;
        }
    }

    ImGui::EndChild();

    // Input Search Bar
    ImGui ::InputTextWithHint("##Search", "Filter Messages", searchString, sizeof(searchString));

    ImGui::NextColumn();

    ImGui::SetColumnWidth(1, 128);

    // Buttons
    if (ImGui::SmallButton("Clear"))
    {
        cubos::core::Logger::clear();
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Collapse"))
    {
        collapseEnabled = !collapseEnabled;
    }
    int warningCount = (int)cubos::core::Logger::warnCount();
    char* warningString = new char[256];
    sprintf(warningString, "Warning (%d)", warningCount);
    if (ImGui::Button(warningString))
    {
        // Add collapse functionality here.
    }

    ImGui::Columns(1); // Reset columns

    ImGui::End();
}

void tesseratos::consolePlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::imguiPlugin);

    cubos.system(console).tagged("cubos.imgui");
}
