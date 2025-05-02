#include "plugin.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/imgui/plugin.hpp>

using cubos::core::net::Address;

using namespace cubos::engine;
using namespace tesseratos;

CUBOS_REFLECT_IMPL(Debugger)
{
    return cubos::core::ecs::TypeBuilder<Debugger>("Debugger").withField("isOpen", &Debugger::isOpen).build();
}

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
    cubos.depends(imguiPlugin);

    cubos.resource<Debugger>();
    cubos.resource<DebuggerSession>();
    cubos.resource<State>();

    cubos.system("show Debugger")
        .onlyIf([](Debugger& tool) { return tool.isOpen; })
        .tagged(imguiTag)
        .call([](State& state, DebuggerSession& debugger, Debugger& tool) {

        if (!ImGui::Begin("Debugger", &tool.isOpen))
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
