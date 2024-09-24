#include "plugin.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/tools/toolbox/plugin.hpp>

using cubos::core::net::Address;

using namespace cubos::engine;

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        std::string address{"127.0.0.1"};
        uint16_t port = 9335;

        uint32_t updateCount = 1;
    };
} // namespace

void tesseratos::debuggerPlugin(Cubos& cubos)
{
    cubos.depends(toolboxPlugin);
    cubos.depends(imguiPlugin);

    cubos.resource<Debugger>();
    cubos.resource<State>();

    cubos.system("show Debugger").tagged(imguiTag).call([](Toolbox& toolbox, State& state, Debugger& debugger) {
        if (!toolbox.isOpen("Debugger"))
        {
            return;
        }

        if (!ImGui::Begin("Debugger"))
        {
            ImGui::End();
            return;
        }

        // If the debugger isn't currently connected, show the connection form UI.
        if (!debugger.isConnected())
        {
            ImGui::InputText("Address", &state.address);
            ImGui::InputScalar("Port", ImGuiDataType_U16, &state.port);

            if (ImGui::Button("Connect"))
            {
                if (auto address = Address::from(state.address))
                {
                    debugger.connect(*address, state.port);
                }
                else
                {
                    CUBOS_ERROR("Failed to parse address from {}", state.address);
                }
            }

            ImGui::End();
            return;
        }

        // Otherwise, show the debugger control UI.

        if (ImGui::Button("Run"))
        {
            debugger.run();
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause"))
        {
            debugger.pause();
        }

        if (ImGui::Button("Update"))
        {
            debugger.update(static_cast<std::size_t>(state.updateCount));
        }
        ImGui::SameLine();
        ImGui::InputScalar("Update count", ImGuiDataType_U32, &state.updateCount);

        if (ImGui::Button("Close"))
        {
            debugger.close();
        }
        ImGui::SameLine();
        if (ImGui::Button("Disconnect"))
        {
            debugger.disconnect();
        }

        ImGui::End();
    });
}
