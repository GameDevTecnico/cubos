/// [Full code]
/// [Including plugin headers]
#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>
/// [Including plugin headers]

using namespace cubos::engine;

/// [ImGui window example]
static void exampleWindowSystem()
{
    ImGui::Begin("Dear ImGui + CUBOS.");
    ImGui::Text("Hello world!");
    ImGui::End();

    ImGui::ShowDemoWindow();
}
/// [ImGui window example]

int main()
{
    Cubos cubos{};

    /// [Adding the plugin]
    cubos.addPlugin(imguiPlugin);
    /// [Adding the plugin]

    /// [Adding the system]
    cubos.system(exampleWindowSystem).tagged("cubos.imgui");
    /// [Adding the system]

    cubos.run();
}
/// [Full code]
