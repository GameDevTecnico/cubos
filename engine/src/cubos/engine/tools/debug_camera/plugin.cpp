#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/renderer/environment.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/renderer/point_light.hpp>
#include <cubos/engine/tools/debug_camera/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::Entity;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

struct DebugCameraInfo
{
    Entity copy[4]; // copy old camera entities to re-apply them in the next camera change
    Entity ent;     // debug camera entity
};

static void createDebugCameraSystem(Commands commands, Write<DebugCameraInfo> debugCamera)
{
    debugCamera->ent = commands.create().add(Camera{}).add(Position{{}}).entity();
}

static void changeToDebugCameraSystem(Write<ActiveCameras> camera, Write<DebugCameraInfo> debugCamera)
{
    ImGui::Begin("Debug Camera");

    ImGui::Text("Current camera: %s", debugCamera->ent == camera->entities[0] ? "Debug" : "Game");
    ImGui::Text("Key to change: F12");

    if (ImGui::Button("Change camera") || ImGui::IsKeyPressed(ImGuiKey_F12))
    {
        CUBOS_DEBUG("Changed camera ...");
        if (debugCamera->ent == camera->entities[0])
        {
            std::copy(std::begin(debugCamera->copy), std::end(debugCamera->copy), camera->entities);
        }
        else
        {
            std::copy(std::begin(camera->entities), std::end(camera->entities), debugCamera->copy);
            camera->entities[0] = debugCamera->ent;
            camera->entities[1] = Entity();
            camera->entities[2] = Entity();
            camera->entities[3] = Entity();
        }
    }

    ImGui::End();
}

void cubos::engine::tools::debugCameraPlugin(Cubos& cubos)
{
    cubos.addPlugin(imguiPlugin);

    cubos.startupSystem(createDebugCameraSystem);
    cubos.system(changeToDebugCameraSystem).tagged("cubos.imgui");

    cubos.addResource<DebugCameraInfo>();
}
