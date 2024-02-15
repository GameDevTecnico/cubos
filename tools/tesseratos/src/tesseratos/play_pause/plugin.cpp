#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>

#include <tesseratos/play_pause/plugin.hpp>
#include <tesseratos/toolbox/plugin.hpp>

using cubos::core::reflection::reflect;

using cubos::engine::Cubos;
using cubos::engine::DeltaTime;

namespace
{
    struct State
    {
        bool paused{false};
        float multiplier{1.0F};
    };
} // namespace

void tesseratos::playPausePlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::imguiPlugin);
    cubos.addPlugin(toolboxPlugin);

    cubos.addResource<State>();

    cubos.system("show Play Pause UI").tagged("cubos.imgui").call([](State& state, Toolbox& toolbox, DeltaTime& dt) {
        if (!toolbox.isOpen("Play Pause"))
        {
            return;
        }

        if (ImGui::Begin("Play Pause"))
        {
            if (ImGui::Button("Play"))
            {
                state.paused = false;
                dt.multiplier = state.multiplier;
            }

            ImGui::SameLine();

            if (ImGui::Button("Pause"))
            {
                state.paused = true;
                dt.multiplier = 0.0;
            }

            ImGui::SameLine();

            ImGui::Text(state.paused ? "(Paused)" : "(Running)");

            ImGui::SliderFloat("Speed Multiplier", &state.multiplier, 0.0F, 5.0F);

            if (ImGui::Button("Reset"))
            {
                state.multiplier = 1.0F;
            }

            ImGui::End();
        }

        if (!state.paused)
        {
            dt.multiplier = state.multiplier;
        }
    });
}
