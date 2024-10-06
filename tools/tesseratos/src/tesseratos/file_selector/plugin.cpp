#include "plugin.hpp"
#include <filesystem>

#include <imgui.h>

#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/memory/standard_stream.hpp>

#include <cubos/engine/imgui/plugin.hpp>

using cubos::core::data::FileSystem;
using cubos::engine::Cubos;

namespace fs = std::filesystem;

using namespace tesseratos;

namespace
{
    struct FileSelectorState
    {
        CUBOS_ANONYMOUS_REFLECT(FileSelectorState);

        // Absolute path to the current directory (for the filesystem not for the virtual filesystem)
        std::string currentPath =
            (fs::current_path().parent_path().parent_path() / "tools" / "tesseratos" / "assets").string();
        // bool isLocal = true to use the virtual filesystem
        bool isLocal = true;
        bool isGridMode = true;
        bool showFileSelector = false;
        std::string selectedFile;
    };
} // namespace

static std::optional<std::string> showFileSelector(FileSelectorState& state)
{
    if (!state.showFileSelector)
    {
        return std::nullopt;
    }

    ImGui::Begin("File Selector");

    // Absolue path for the virtual filesystem
    std::string relativePath =
        fs::relative(state.currentPath, fs::current_path().parent_path().parent_path() / "tools" / "tesseratos")
            .string();

    std::replace(relativePath.begin(), relativePath.end(), '\\', '/');

    ImGui::Text("Current Path: %s", state.isLocal ? std::string(relativePath).c_str() : state.currentPath.c_str());

    ImGui::Separator();

    // Search bar
    static char searchBuffer[256] = "";
    ImGui::InputTextWithHint("##search", "Search...", searchBuffer, sizeof(searchBuffer));

    ImGui::SameLine();

    // Parent directory button
    if (!(state.isLocal && relativePath == "assets"))
    {
        if (ImGui::Button("Up"))
        {
            state.currentPath = fs::path(state.currentPath).parent_path().string();
        }
    }

    ImGui::SameLine();

    // View mode toggle
    if (ImGui::RadioButton("Grid", state.isGridMode))
    {
        state.isGridMode = true;
    }

    ImGui::SameLine();

    if (ImGui::RadioButton("List", !state.isGridMode))
    {
        state.isGridMode = false;
    }

    ImGui::Separator();

    float cellSize = 100.0F; // Size of each grid cell
    float padding = 10.0F;   // Padding between cells

    if (state.isGridMode)
    {
        // Calculate number of columns for grid layout
        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columns = (int)(panelWidth / (cellSize + padding));
        if (columns < 1)
        {
            columns = 1;
        }
        ImGui::Columns(columns, nullptr, false);
    }

    for (const auto& entry : fs::directory_iterator(state.currentPath))
    {
        // Filter files and directories based on search query
        std::string fileName = entry.path().filename().string();
        if (strlen(searchBuffer) > 0 && fileName.find(searchBuffer) == std::string::npos)
            continue;

        if (entry.is_directory())
        {
            // Directories
            if (ImGui::Button((fileName + "/").c_str(), state.isGridMode ? ImVec2(cellSize, cellSize) : ImVec2(0, 0)))
            {
                state.currentPath = entry.path().string();
            }
        }
        else if (entry.is_regular_file())
        {
            // Files
            if (ImGui::Selectable(fileName.c_str(), state.selectedFile == entry.path().string(), 0,
                                  state.isGridMode ? ImVec2(cellSize, cellSize) : ImVec2(0, 0)))
            {
                std::string selectedFilePath = entry.path().string();
                if (state.isLocal)
                {
                    selectedFilePath = fs::relative(selectedFilePath, fs::current_path().parent_path().parent_path() /
                                                                          "tools" / "tesseratos")
                                           .string();
                    std::replace(selectedFilePath.begin(), selectedFilePath.end(), '\\', '/');
                    state.selectedFile = selectedFilePath;
                }
                else
                {
                    state.selectedFile = selectedFilePath;
                }
            }
        }

        if (state.isGridMode)
            ImGui::NextColumn(); // Move to next column in grid layout
    }

    if (state.isGridMode)
        ImGui::Columns(1); // End grid layout

    ImGui::Separator();

    // Display selected file
    if (!state.selectedFile.empty())
    {
        ImGui::Text("Selected File: %s", state.selectedFile.c_str());

        // Confirm button
        if (ImGui::Button("Select"))
        {
            ImGui::End();
            std::string selectedFile = state.selectedFile;
            state.selectedFile.clear();
            state.showFileSelector = false;
            return selectedFile;
        }
    }
    else
    {
        ImGui::Text("No file selected.");
    }

    ImGui::End();
    return std::nullopt;
}

static std::optional<std::string> createFileSelectorButton(FileSelectorState& state, const char* label)
{
    if (ImGui::Button(label))
    {
        state.showFileSelector = true;
    }

    auto selectedFile = showFileSelector(state);
    return selectedFile;
}

void tesseratos::fileSelectorPlugin(Cubos& cubos)
{
    cubos.depends(cubos::engine::imguiPlugin);
    cubos.resource<FileSelectorState>();

    cubos.system("show File Selector UI").tagged(cubos::engine::imguiTag).call([](FileSelectorState& state) {
        ImGui::Begin("File Selector");
        createFileSelectorButton(state, "Open File Selector");
        ImGui::End();
    });
}
