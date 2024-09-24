#include <imgui.h>

#include <cubos/core/log.hpp>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/tools/console/plugin.hpp>
#include <cubos/engine/tools/toolbox/plugin.hpp>

namespace
{
    // Resource used to store the state of the console
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        std::vector<cubos::core::Logger::Entry> uiEntries;
        std::size_t cursor = 0;
        char searchString[256];
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

// Function to convert log level to a string
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

// Function to convert log level to a color
static ImVec4 levelToColor(cubos::core::Logger::Level level)
{
    switch (level)
    {
    case cubos::core::Logger::Level::Trace:
        return {1.0F, 1.0F, 1.0F, 1.0F}; // White
    case cubos::core::Logger::Level::Debug:
        return {0.0F, 1.0F, 1.0F, 1.0F}; // Cyan
    case cubos::core::Logger::Level::Info:
        return {0.0F, 1.0F, 0.0F, 1.0F}; // Green
    case cubos::core::Logger::Level::Warn:
        return {1.0F, 1.0F, 0.0F, 1.0F}; // Yellow
    case cubos::core::Logger::Level::Error:
        return {1.0F, 0.0F, 0.0F, 1.0F}; // Red
    case cubos::core::Logger::Level::Critical:
        return {1.0F, 0.0F, 1.0F, 1.0F}; // Magenta
    default:
        return {1.0F, 1.0F, 1.0F, 1.0F}; // Default to white
    }
}

void cubos::engine::consolePlugin(Cubos& cubos)
{
    cubos.depends(imguiPlugin);
    cubos.depends(toolboxPlugin);

    cubos.resource<State>();

    cubos.system("show Logger UI").tagged(imguiTag).call([](Toolbox& toolbox, State& state) {
        if (!toolbox.isOpen("Console"))
        {
            return;
        }

        ImGui::SetNextWindowSize(ImVec2(930, 310), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::Begin("Console");

        ImGui::Columns(2, "ConsoleLayout", true);

        // Log Scrollable Window
        ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.8F);

        cubos::core::Logger::Entry entry;
        cubos::core::Logger::Entry previousEntry;

        // Counters for each log level
        std::unordered_map<cubos::core::Logger::Level, int> logCounts = {
            {cubos::core::Logger::Level::Trace, 0}, {cubos::core::Logger::Level::Debug, 0},
            {cubos::core::Logger::Level::Info, 0},  {cubos::core::Logger::Level::Warn, 0},
            {cubos::core::Logger::Level::Error, 0}, {cubos::core::Logger::Level::Critical, 0}};

        ImGui::BeginChild("Log", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

        while (cubos::core::Logger::read(state.cursor, entry))
        {
            state.uiEntries.push_back(entry);
        }

        if (state.clearEnabled)
        {
            state.clearEnabled = false;
            state.uiEntries.clear();
        }

        std::size_t counter = 0;
        bool isFirstEntry = true;
        for (cubos::core::Logger::Entry& e : state.uiEntries)
        {
            if (e.level != cubos::core::Logger::Level::Off)
            {
                logCounts[e.level]++;
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

            if (state.searchString[0] != '\0' && e.message.find(state.searchString) == std::string::npos)
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
        ImGui ::InputTextWithHint("##Search", "Filter Messages", state.searchString, sizeof(state.searchString));

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

        std::string traceText = "Trace (" + std::to_string(logCounts[cubos::core::Logger::Level::Trace]) + ")";
        if (ImGui::SmallButton(traceText.c_str()))
        {
            state.traceEnabled = !state.traceEnabled;
        }

        std::string debugText = "Debug (" + std::to_string(logCounts[cubos::core::Logger::Level::Debug]) + ")";
        if (ImGui::SmallButton(debugText.c_str()))
        {
            state.debugEnabled = !state.debugEnabled;
        }

        std::string infoText = "Info (" + std::to_string(logCounts[cubos::core::Logger::Level::Info]) + ")";
        if (ImGui::SmallButton(infoText.c_str()))
        {
            state.infoEnabled = !state.infoEnabled;
        }

        std::string warnText = "Warn (" + std::to_string(logCounts[cubos::core::Logger::Level::Warn]) + ")";
        if (ImGui::SmallButton(warnText.c_str()))
        {
            state.warnEnabled = !state.warnEnabled;
        }

        std::string errorText = "Error (" + std::to_string(logCounts[cubos::core::Logger::Level::Error]) + ")";
        if (ImGui::SmallButton(errorText.c_str()))
        {
        }

        std::string criticalText = "Critical (" + std::to_string(logCounts[cubos::core::Logger::Level::Critical]) + ")";
        if (ImGui::SmallButton(criticalText.c_str()))
        {
            state.criticalEnabled = !state.criticalEnabled;
        }

        ImGui::Columns(1); // Reset columns

        ImGui::End();
    });
}
