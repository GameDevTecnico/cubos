#include <unordered_set>

#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/perspective.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/tools/debug_camera/plugin.hpp>
#include <cubos/engine/tools/toolbox/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>

using namespace cubos::core::io;
using cubos::core::ecs::Entity;
using cubos::core::ecs::EntityHash;

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        // Cameras which were deactivated by the plugin.
        std::unordered_set<Entity, EntityHash> deactivated;

        // Entity of the debug camera.
        Entity entity;

        // Whether the debug camera is currently active.
        bool active;

        // Whether the debug camera's toggle mode is currently active.
        bool toggled;

        // Whether the debug camera's hold mode is currently active.
        bool holding;
    };
} // namespace

void cubos::engine::debugCameraPlugin(Cubos& cubos)
{
    cubos.depends(imguiPlugin);
    cubos.depends(freeCameraPlugin);
    cubos.depends(cameraPlugin);
    cubos.depends(renderTargetPlugin);
    cubos.depends(inputPlugin);
    cubos.depends(toolboxPlugin);

    cubos.resource<State>();

    cubos.startupSystem("create Debug Camera").call([](Commands commands, State& debugCamera) {
        debugCamera.entity = commands.create()
                                 .named("debug-camera")
                                 .add(Camera{.active = false})
                                 .add(PerspectiveCamera{})
                                 .add(Position{{}})
                                 .add(FreeCameraController{
                                     .enabled = false,
                                     .unscaledDeltaTime = true,
                                     .lateral = "debug-move-lateral",
                                     .vertical = "debug-move-vertical",
                                     .longitudinal = "debug-move-longitudinal",
                                 })
                                 .entity();
    });

    cubos.system("toggle Debug Camera controller")
        .call([](Input& input, State& state, Query<FreeCameraController&> entities) {
            if (auto match = entities.at(state.entity); match && state.active && !state.holding)
            {
                auto [controller] = *match;
                if (input.justPressed("debug-camera-toggle"))
                {
                    state.toggled = !state.toggled;
                    controller.enabled = !controller.enabled;
                }
            }
        });

    cubos.system("hold Debug Camera controller")
        .call([](Input& input, State& state, Query<FreeCameraController&> entities) {
            if (auto match = entities.at(state.entity); match && state.active && !state.toggled)
            {
                auto [controller] = *match;
                if (input.pressed("debug-camera-hold"))
                {
                    state.holding = true;
                    controller.enabled = true;
                }
                else
                {
                    state.holding = false;
                    controller.enabled = false;
                }
            }
        });

    cubos.system("update Debug Camera speed")
        .call([](Input& input, Query<FreeCameraController&> entities, const cubos::core::ecs::DeltaTime& deltaTime) {
            for (auto [controller] : entities)
            {
                if (controller.enabled)
                {
                    // Speed up exponentially, taking the delta time into account.
                    float changeSpeed = input.axis("debug-change-speed");
                    controller.speed *= glm::pow(2.0F, changeSpeed * deltaTime.value() * 4.0F);
                    controller.speed = glm::clamp(controller.speed, 0.01F, 1000000.0F);
                }
            }
        });

    cubos.system("update Debug Camera")
        .tagged(imguiTag)
        .call([](State& state, Toolbox& toolbox, Commands cmds,
                 Query<Entity, Camera&, const DrawsTo&, const RenderTarget&> cameras,
                 Query<Entity, const RenderTarget&> targets) {
            if (toolbox.isOpen("Debug Camera"))
            {
                ImGui::Begin("Debug Camera");

                ImGui::Text("Current camera: %s", state.active ? "Debug" : "Game");
                if (ImGui::Button("Toggle camera"))
                {
                    state.active = !state.active;
                }

                ImGui::End();
            }

            if (ImGui::IsKeyPressed(ImGuiKey_F9))
            {
                state.active = !state.active;
            }

            if (cameras.pin(0, state.entity).empty())
            {
                for (auto [targetEnt, target] : targets)
                {
                    if (target.framebuffer == nullptr)
                    {
                        cmds.relate(state.entity, targetEnt, DrawsTo{});
                    }
                }
            }

            for (auto [cameraEnt, camera, _1, target] : cameras)
            {
                if (target.framebuffer == nullptr)
                {
                    if (cameraEnt == state.entity)
                    {
                        camera.active = state.active;
                    }
                    else if (camera.active && state.active)
                    {
                        state.deactivated.insert(cameraEnt);
                        camera.active = false;
                    }
                    else if (!state.active && state.deactivated.contains(cameraEnt))
                    {
                        state.deactivated.erase(cameraEnt);
                        camera.active = true;
                    }
                }
            }
        });
}
