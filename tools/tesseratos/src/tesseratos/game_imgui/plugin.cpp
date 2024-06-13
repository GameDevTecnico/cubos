#include "plugin.hpp"

#include <imgui.h>

#include "../game/plugin.hpp"

using namespace cubos::engine;

void tesseratos::gameImGuiPlugin(Cubos& cubos)
{
    cubos.depends(gamePlugin);

    cubos.startupSystem("add ImGui context switch to Game").call([](Game& game) {
        game.addContextSwitch([out = (ImGuiContext*)nullptr, in = (ImGuiContext*)nullptr](bool enterGame) mutable {
            if (enterGame)
            {
                out = ImGui::GetCurrentContext();
                ImGui::SetCurrentContext(in);
            }
            else
            {
                in = ImGui::GetCurrentContext();
                ImGui::SetCurrentContext(out);
            }
        });
    });
}
