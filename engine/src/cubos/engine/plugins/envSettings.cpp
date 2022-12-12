#include <cubos/core/settings.hpp>
#include <cubos/engine/plugins/envSettings.hpp>

static void startup()
{
    printf("[dbg] startup from env settings plugin !!!\n");
}

void cubos::engine::plugins::envSettingsPlugin(Cubos& cubos)
{
    cubos
        //.addResource<cubos::core::Settings>() // FIXME: do we add resource here, or is it CUBOS' ctor job?
        .addStartupSystem(startup, "Settings");
}
