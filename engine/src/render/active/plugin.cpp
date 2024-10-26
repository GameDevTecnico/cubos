#include <cubos/engine/render/active/active.hpp>
#include <cubos/engine/render/active/plugin.hpp>

void cubos::engine::activePlugin(Cubos& cubos)
{
    cubos.component<Active>();
}