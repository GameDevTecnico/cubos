#include <cubos/engine/cubos.hpp>

#include <cubos/engine/plugins/window.hpp>
#include <cubos/engine/plugins/env_settings.hpp>

#include <cubos/core/ui/imgui.hpp>
#include <cubos/core/ui/serialization.hpp>
#include <imgui.h>

static void setup(const cubos::core::io::Window& window, ShouldQuit& quit)
{
    cubos::core::ui::initialize(window);
}

static void show(const cubos::core::io::Window& window)
{
    cubos::core::ui::beginFrame();

    ImGui::Begin("Editor");
    ImGui::End();

    cubos::core::ui::endFrame();
}

int main(int argc, char** argv)
{
    // FIXME: this will change once we have the imgui/editor plugin!!!
    Cubos(argc, argv)
        .addPlugin(cubos::engine::plugins::windowPlugin)

        .startupSystem(setup)
        .tagged("SetupUI")

        .system(show)
        .tagged("showUI")

        .run();
}
