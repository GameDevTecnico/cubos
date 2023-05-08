#include <imgui.h>

#include <cubos/core/ui/imgui.hpp>
#include <cubos/core/ui/serialization.hpp>

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/env_settings/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::ecs::Read;
using cubos::core::io::Window;
using namespace cubos::engine;

static void setup(Read<Window> window)
{
    cubos::core::ui::initialize(*window);
}

static void show()
{
    cubos::core::ui::beginFrame();

    ImGui::Begin("Editor");
    ImGui::End();

    cubos::core::ui::endFrame();
}

int main(int argc, char** argv)
{
    // FIXME: this will change once we have the imgui/editor plugin!!!
    Cubos cubos(argc, argv);

    cubos.addPlugin(cubos::engine::windowPlugin);

    cubos.startupSystem(setup).tagged("SetupUI");

    cubos.system(show).tagged("showUI");

    cubos.run();
}
