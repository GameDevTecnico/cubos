#include <imgui.h>

#include <cubos/core/ecs/name.hpp>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/renderer/environment.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/renderer/point_light.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>

#include <tesseratos/debug_camera/plugin.hpp>
#include <tesseratos/toolbox/plugin.hpp>

using namespace cubos::core::io;

using cubos::core::ecs::Name;

using cubos::engine::ActiveCameras;
using cubos::engine::Camera;
using cubos::engine::Commands;
using cubos::engine::Cubos;
using cubos::engine::Entity;
using cubos::engine::FreeCameraController;
using cubos::engine::Input;
using cubos::engine::Position;
using cubos::engine::Query;

namespace
{
    struct DebugCameraInfo
    {
        Entity copy[4]; // copy old camera entities to re-apply them in the next camera change
        Entity ent;     // debug camera entity
        bool justChanged = false;
    };
} // namespace

void tesseratos::debugCameraPlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::imguiPlugin);
    cubos.addPlugin(toolboxPlugin);
    cubos.addPlugin(cubos::engine::freeCameraPlugin);

    cubos.addResource<DebugCameraInfo>();

    cubos.startupSystem("create Debug Camera")
        .call([](Commands commands, DebugCameraInfo& debugCamera) {
            debugCamera.ent = commands.create()
                                  .add(Name{"debug-camera"})
                                  .add(Camera{})
                                  .add(Position{{}})
                                  .add(FreeCameraController{
                                      .enabled = false,
                                      .lateral = "debug-move-lateral",
                                      .vertical = "debug-move-vertical",
                                      .longitudinal = "debug-move-longitudinal",
                                  })
                                  .entity();
        });

    cubos.system("toggle Debug Camera")
        .call([](Input& input, DebugCameraInfo& info, Query<FreeCameraController&> entities,
                 const ActiveCameras& cameras) {
            if (auto match = entities.at(info.ent); match && cameras.entities[0] == info.ent)
            {
                auto [controller] = *match;
                if (!input.pressed("debug-camera-toggle"))
                {
                    info.justChanged = false;
                }
                else if (!info.justChanged)
                {
                    info.justChanged = true;
                    controller.enabled = !controller.enabled;
                }
            }
        });

    cubos.system("show Debug Camera UI")
        .tagged(cubos::engine::imguiTag)
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

    cubos.system("fallback to Debug Camera").call([](ActiveCameras& cameras, DebugCameraInfo& debugCamera) {
        if (cameras.entities[0].isNull())
        {
            std::copy(std::begin(cameras.entities), std::end(cameras.entities), debugCamera.copy);
            cameras.entities[0] = debugCamera.ent;
        }
    });
}
