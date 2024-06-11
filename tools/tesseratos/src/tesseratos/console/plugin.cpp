#include <imgui.h>

#include <cubos/core/log.hpp>

#include <cubos/engine/imgui/plugin.hpp>

#include <tesseratos/console/plugin.hpp>
#include <tesseratos/toolbox/plugin.hpp>

using cubos::engine::Cubos;

using namespace tesseratos;

namespace
{
    struct State
    {
        std::vector<cubos::core::Logger::Entry> ui_entries;
        std::size_t prevCursor = 0; // Previous cursor position~could find a better way to clear log
        bool clearEnabled = false;
        bool collapseEnabled = false;
        bool traceEnabled = true;
        bool debugEnabled = true;
        bool infoEnabled = true;
        bool warnEnabled = true;
        bool errorEnabled = true;
        bool criticalEnabled = true;
    };
} // namespace

static std::string levelToString(cubos::core::Logger::Level level)
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
    default:
        return "Unknown";
    }
}

static ImVec4 levelToColor(cubos::core::Logger::Level level)
{
    switch (level)
    {
    case cubos::core::Logger::Level::Trace:
        return ImVec4(1.0F, 1.0F, 1.0F, 1.0F); // White
    case cubos::core::Logger::Level::Debug:
        return ImVec4(0.0F, 1.0F, 1.0F, 1.0F); // Cyan
    case cubos::core::Logger::Level::Info:
        return ImVec4(0.0F, 1.0F, 0.0F, 1.0F); // Green
    case cubos::core::Logger::Level::Warn:
        return ImVec4(1.0F, 1.0F, 0.0F, 1.0F); // Yellow
    case cubos::core::Logger::Level::Error:
        return ImVec4(1.0F, 0.0F, 0.0F, 1.0F); // Red
    case cubos::core::Logger::Level::Critical:
        return ImVec4(1.0F, 0.0F, 0.0F, 0.5F); // Bright Red
    default:
        return {1.0F, 1.0F, 1.0F, 1.0F}; // Default to
    }
}

void tesseratos::consolePlugin(Cubos& cubos)
{
    cubos.depends(cubos::engine::imguiPlugin);
    cubos.depends(toolboxPlugin);
    cubos.resource<State>();

    cubos.system("show Logger UI").tagged(cubos::engine::imguiTag).call([](Toolbox& toolbox, State& state) {
        if (!toolbox.isOpen("Console"))
        {
            return;
        }

        static char searchString[256];

        ImGui::SetNextWindowSize(ImVec2(930, 310), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::Begin("Console");

        ImGui::Columns(2, "ConsoleLayout", true);

        // Log Scrollable Window
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.8f);

        std::size_t cursor = state.prevCursor;
        cubos::core::Logger::Entry entry;
        cubos::core::Logger::Entry previousEntry;

        // Counters for each log level
        std::size_t traceCount = 0;
        std::size_t debugCount = 0;
        std::size_t infoCount = 0;
        std::size_t warnCount = 0;
        std::size_t errorCount = 0;
        std::size_t criticalCount = 0;

        ImGui::BeginChild("Log", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

        state.ui_entries.clear();

        while (cubos::core::Logger::read(cursor, entry))
        {
            state.ui_entries.push_back(entry);
        }

        if (state.clearEnabled)
        {
            state.clearEnabled = false;
            state.prevCursor = cursor;
        }

        std::size_t counter = 0;
        bool isFirstEntry = true;
        for (cubos::core::Logger::Entry& e : state.ui_entries)
        {
            switch (e.level)
            {
            case cubos::core::Logger::Level::Trace:
                traceCount++;
                break;
            case cubos::core::Logger::Level::Debug:
                debugCount++;
                break;
            case cubos::core::Logger::Level::Info:
                infoCount++;
                break;
            case cubos::core::Logger::Level::Warn:
                warnCount++;
                break;
            case cubos::core::Logger::Level::Error:
                errorCount++;
                break;
            case cubos::core::Logger::Level::Critical:
                criticalCount++;
                break;
            case cubos::core::Logger::Level::Off:
                break;
            }

            switch (e.level)
            {
            case cubos::core::Logger::Level::Trace:
                if (!state.traceEnabled)
                {
                    continue;
                }
                break;
            case cubos::core::Logger::Level::Debug:
                if (!state.debugEnabled)
                {
                    continue;
                }
                break;
            case cubos::core::Logger::Level::Info:
                if (!state.infoEnabled)
                {
                    continue;
                }
                break;
            case cubos::core::Logger::Level::Warn:
                if (!state.warnEnabled)
                {
                    continue;
                }
                break;
            case cubos::core::Logger::Level::Error:
                if (!state.errorEnabled)
                {
                    continue;
                }
                break;
            case cubos::core::Logger::Level::Critical:
                if (!state.criticalEnabled)
                {
                    continue;
                }
                break;
            case cubos::core::Logger::Level::Off:
                break;
            }

            if (searchString[0] != '\0' && e.message.find(searchString) == std::string::npos)
            {
                continue;
            }

            if (state.collapseEnabled)
            {
                if (!isFirstEntry && previousEntry.message == e.message)
                {
                    counter++;
                    continue;
                }
                else
                {
                    if (!isFirstEntry && counter > 0)
                    {
                        ImGui::TextColored(levelToColor(previousEntry.level), "[%s] %s (x%zu)",
                                           levelToString(previousEntry.level).c_str(), previousEntry.message.c_str(),
                                           counter + 1);
                        counter = 0;
                    }
                    else if (!isFirstEntry)
                    {
                        ImGui::TextColored(levelToColor(previousEntry.level), "[%s] %s",
                                           levelToString(previousEntry.level).c_str(), previousEntry.message.c_str());
                    }
                }
                previousEntry = e;
                isFirstEntry = false;
            }
            else
            {
                ImGui::TextColored(levelToColor(e.level), "[%s] %s", levelToString(e.level).c_str(), e.message.c_str());
            }
        }

        ImGui::EndChild();

        // Input Search Bar
        ImGui ::InputTextWithHint("##Search", "Filter Messages", searchString, sizeof(searchString));

        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, 128);

        // Clear Button
        if (ImGui::SmallButton("Clear"))
        {
            state.clearEnabled = true;
        }

        // Collapse Button
        if (ImGui::SmallButton("Collapse"))
        {
            state.collapseEnabled = !state.collapseEnabled;
        }

        // Log Level Counters
        // ImGui SELECTABLE
        std::string traceText = "Trace (" + std::to_string(traceCount) + ")";
        if (ImGui::SmallButton(traceText.c_str()))
        {
            state.traceEnabled = !state.traceEnabled;
        }

        std::string debugText = "Debug (" + std::to_string(debugCount) + ")";
        if (ImGui::SmallButton(debugText.c_str()))
        {
            state.debugEnabled = !state.debugEnabled;
        }

        std::string infoText = "Info (" + std::to_string(infoCount) + ")";
        if (ImGui::SmallButton(infoText.c_str()))
        {
            state.infoEnabled = !state.infoEnabled;
        }

        std::string warnText = "Warn (" + std::to_string(warnCount) + ")";
        if (ImGui::SmallButton(warnText.c_str()))
        {
            state.warnEnabled = !state.warnEnabled;
        }

        std::string errorText = "Error (" + std::to_string(errorCount) + ")";
        if (ImGui::SmallButton(errorText.c_str()))
        {
        }

        std::string criticalText = "Critical (" + std::to_string(criticalCount) + ")";
        if (ImGui::SmallButton(criticalText.c_str()))
        {
            state.criticalEnabled = !state.criticalEnabled;
        }

        ImGui::Columns(1); // Reset columns

        ImGui::End();
    });
}
