#include <imgui.h>

#include <cubos/core/log.hpp>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/imgui/serialization.hpp>

#include <tesseratos/console/plugin.hpp>

using cubos::engine::Cubos;

using namespace tesseratos;

static void console()
{
}

void tesseratos::consolePlugin(Cubos& cubos)
{
    cubos.addPlugin(cubos::engine::imguiPlugin);

    cubos.system(console).tagged("cubos.imgui");
}
