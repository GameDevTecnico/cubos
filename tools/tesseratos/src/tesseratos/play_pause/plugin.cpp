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
        float scale{1.0F};
    };
} // namespace

void tesseratos::playPausePlugin(Cubos& cubos)
{
    cubos.depends(cubos::engine::imguiPlugin);
    cubos.depends(toolboxPlugin);

    cubos.resource<State>();

    cubos.system("show Play Pause UI")
        .tagged(cubos::engine::imguiTag)
        .call([](State& state, Toolbox& toolbox, DeltaTime& dt) {
            if (!toolbox.isOpen("Play Pause"))
            {
                return;
            }

            if (ImGui::Begin("Play Pause"))
            {
                if (ImGui::Button("Play"))
                {
                    state.paused = false;
                    dt.scale = state.scale;
                }

                ImGui::SameLine();

                if (ImGui::Button("Pause"))
                {
                    state.paused = true;
                    dt.scale = 0.0;
                }

                ImGui::SameLine();

                ImGui::Text(state.paused ? "(Paused)" : "(Running)");

                ImGui::SliderFloat("Speed Scale", &state.scale, 0.0F, 5.0F);

                if (ImGui::Button("Reset"))
                {
                    state.scale = 1.0F;
                }

                ImGui::End();
            }

            if (!state.paused)
            {
                dt.scale = state.scale;
            }
        });
}
