#include "plugin.hpp"
#include <sstream>

#include <imgui.h>
#include <imgui_stdlib.h>

#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/data/fs/standard_archive.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>

#include "../debugger/plugin.hpp"

using namespace cubos::core::data;
using namespace cubos::engine;

using namespace tesseratos;

CUBOS_REFLECT_IMPL(Project)
{
    return cubos::core::ecs::TypeBuilder<Project>("Project").withField("isOpen", &Project::isOpen).build();
}

void tesseratos::projectPlugin(Cubos& cubos)
{
    cubos.depends(imguiPlugin);
    cubos.depends(settingsPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(debuggerPlugin);

    cubos.resource<Project>();
    cubos.resource<ProjectManager::State>();

    cubos.system("show Project")
        .onlyIf([](Project& tool) { return tool.isOpen; })
        .tagged(imguiTag)
        .call([](ProjectManager project, Settings& settings, Project& tool) {

            if (!ImGui::Begin("Project", &tool.isOpen))
            {
                ImGui::End();
                return;
            }

            if (!project.open())
            {
                // If the project is not open, only show the open dialog.
                auto projectOSPath = settings.getString("project.path", "");
                if (ImGui::InputText("Project Path", &projectOSPath))
                {
                    settings.setString("project.path", projectOSPath);
                }

                auto binaryOSPath = settings.getString("project.binary.path", "");
                if (ImGui::InputText("Binary Path", &binaryOSPath))
                {
                    settings.setString("project.binary.path", binaryOSPath);
                }

                if (ImGui::Button("Open") && !project.open(projectOSPath, binaryOSPath))
                {
                    CUBOS_ERROR("Failed to open project");
                }

                ImGui::End();
                return;
            }

            if (ImGui::Button("Launch"))
            {
                project.launch();
            }

            if (ImGui::Button("Terminate"))
            {
                project.terminate();
            }

            if (ImGui::Button("Close"))
            {
                project.close();
            }

            ImGui::End();
        });
}
