#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/renderer/environment.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/renderer/point_light.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include <tesseratos/debug_camera/plugin.hpp>
#include <tesseratos/toolbox/plugin.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::Entity;

using cubos::engine::ActiveCameras;
using cubos::engine::Camera;
using cubos::engine::Cubos;
using cubos::engine::Position;

namespace
{
    struct DebugCameraInfo
    {
        Entity copy[4]; // copy old camera entities to re-apply them in the next camera change
        Entity ent;     // debug camera entity
    };
} // namespace

void tesseratos::debugCameraPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::imguiPlugin);
    cubos.addPlugin(toolboxPlugin);

    cubos.startupSystem("create Debug Camera").call([](Commands commands, DebugCameraInfo& debugCamera) {
        debugCamera.ent = commands.create().add(Camera{}).add(Position{{}}).entity();
    });

    cubos.system("show Debug Camera UI")
        .tagged("cubos.imgui")
        .call([](ActiveCameras& camera, Toolbox& toolbox, DebugCameraInfo& debugCamera) {
            if (!toolbox.isOpen("Debug Camera"))
            {
                return;
            }

            ImGui::Begin("Debug Camera");

            ImGui::Text("Current camera: %s", debugCamera.ent == camera.entities[0] ? "Debug" : "Game");
            ImGui::Text("Key to change: F12");

            if (ImGui::Button("Change camera") || ImGui::IsKeyPressed(ImGuiKey_F12))
            {
                CUBOS_DEBUG("Changed camera ...");
                if (debugCamera.ent == camera.entities[0])
                {
                    std::copy(std::begin(debugCamera.copy), std::end(debugCamera.copy), camera.entities);
                }
                else
                {
                    std::copy(std::begin(camera.entities), std::end(camera.entities), debugCamera.copy);
                    camera.entities[0] = debugCamera.ent;
                    camera.entities[1] = Entity();
                    camera.entities[2] = Entity();
                    camera.entities[3] = Entity();
                }
            }

            ImGui::End();
        });

    cubos.addResource<DebugCameraInfo>();
}
