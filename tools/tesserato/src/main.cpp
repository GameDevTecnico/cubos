#include <imgui.h>

#include <cubos/engine/imgui/plugin.hpp>

using namespace cubos::engine;

static void show()
{
    ImGui::Begin("Editor");
    ImGui::End();
}

int main(int argc, char** argv)
{
    Cubos cubos(argc, argv);
    cubos.addPlugin(imguiPlugin);
    cubos.system(show).tagged("cubos.imgui");
    cubos.run();
}
