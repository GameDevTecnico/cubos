#include "plugin.hpp"
#include <unordered_set>

#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/perspective_camera.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>

#include "../toolbox/plugin.hpp"

using namespace cubos::core::io;

using cubos::core::ecs::EntityHash;

using cubos::engine::Commands;
using cubos::engine::Cubos;
using cubos::engine::DrawsTo;
using cubos::engine::Entity;
using cubos::engine::FreeCameraController;
using cubos::engine::Input;
using cubos::engine::PerspectiveCamera;
using cubos::engine::Position;
using cubos::engine::Query;
using cubos::engine::RenderTarget;

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
    };
} // namespace

void tesseratos::debugCameraPlugin(Cubos& cubos)
{
    cubos.depends(cubos::engine::imguiPlugin);
    cubos.depends(cubos::engine::freeCameraPlugin);
    cubos.depends(cubos::engine::cameraPlugin);
    cubos.depends(cubos::engine::renderTargetPlugin);
    cubos.depends(cubos::engine::inputPlugin);

    cubos.depends(toolboxPlugin);

    cubos.resource<State>();

    cubos.startupSystem("create Debug Camera").call([](Commands commands, State& debugCamera) {
        debugCamera.entity = commands.create()
                                 .named("debug-camera")
                                 .add(PerspectiveCamera{
                                     .active = false,
                                 })
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
            if (auto match = entities.at(state.entity); match && state.active)
            {
                auto [controller] = *match;
                if (input.justPressed("debug-camera-toggle"))
                {
                    controller.enabled = !controller.enabled;
                }
            }
        });

    cubos.system("update Debug Camera speed")
        .call([](Input& input, Query<FreeCameraController&> entities, const cubos::core::ecs::DeltaTime& deltaTime) {
            for (auto [controller] : entities)
            {
                if (controller.enabled)
                {
                    controller.speed += input.axis("debug-change-speed") * deltaTime.value();
                }
            }
        });

    cubos.system("update Debug Camera")
        .tagged(cubos::engine::imguiTag)
        .onlyIf([](Toolbox& toolbox) { return toolbox.isOpen("Debug Camera"); })
        .call([](State& state, Commands cmds,
                 Query<Entity, PerspectiveCamera&, const DrawsTo&, const RenderTarget&> cameras,
                 Query<Entity, const RenderTarget&> targets) {
            ImGui::Begin("Debug Camera");

            ImGui::Text("Current camera: %s", state.active ? "Debug" : "Game");
            if (ImGui::Button("Toggle camera"))
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

            ImGui::End();
        });
}
