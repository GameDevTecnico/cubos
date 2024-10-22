#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/tools/play_pause/plugin.hpp>
#include <cubos/engine/tools/toolbox/plugin.hpp>

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        bool paused{false};
        float scale{1.0F};
    };
} // namespace

void cubos::engine::playPauseToolPlugin(Cubos& cubos)
{
    cubos.depends(imguiPlugin);
    cubos.depends(toolboxPlugin);

    cubos.resource<State>();

    cubos.system("show Play Pause tool UI").tagged(imguiTag).call([](State& state, Toolbox& toolbox, DeltaTime& dt) {
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
        }
        ImGui::End();

        if (!state.paused)
        {
            dt.scale = state.scale;
        }
    });
}
