#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>

#include <tesseratos/pause/plugin.hpp>

using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace tesseratos;

namespace
{
    /// @brief Resource which stores whether pause is enabled.
    struct State
    {
        bool paused;
    };
} // namespace

static void toggleSystem(Write<State> state)
{
    ImGui::Begin("State");

    if (state->paused)
    {
        if (ImGui::Button("Resume"))
        {
            state->paused = false;
        }
    }
    else
    {
        if (ImGui::Button("Pause"))
        {
            state->paused = true;
        }
    }

    ImGui::End();
}

static bool unpausedCondition(Read<State> state)
{
    return !state->paused;
}

void tesseratos::pausePlugin(cubos::engine::Cubos& cubos)
{
    cubos.named("tesseratos::pausePlugin");

    cubos.addPlugin(cubos::engine::imguiPlugin);
    cubos.addPlugin(cubos::engine::rendererPlugin);

    cubos.addResource<State>(State{.paused = false});

    cubos.system(toggleSystem).tagged("cubos.imgui");

    cubos.tag("cubos::engine::assetsPlugin").tagged("tesseratos.pause.ignore");
    cubos.tag("cubos::engine::gizmosPlugin").tagged("tesseratos.pause.ignore");
    cubos.tag("cubos::engine::imguiPlugin").tagged("tesseratos.pause.ignore");
    cubos.tag("cubos::engine::inputPlugin").tagged("tesseratos.pause.ignore");
    cubos.tag("cubos::engine::rendererPlugin").tagged("tesseratos.pause.ignore");
    cubos.tag("cubos::engine::transformPlugin").tagged("tesseratos.pause.ignore");
    cubos.tag("cubos::engine::windowPlugin").tagged("tesseratos.pause.ignore");
    cubos.tag("tesseratos").tagged("tesseratos.pause.ignore");

    cubos.noTag("tesseratos.pause.ignore").runIf(unpausedCondition);
}
