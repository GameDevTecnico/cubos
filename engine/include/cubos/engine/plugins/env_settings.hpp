#ifndef CUBOS_ENGINE_PLUGINS_ENV_SETTINGS_HPP
#define CUBOS_ENGINE_PLUGINS_ENV_SETTINGS_HPP

#include <cubos/engine/cubos.hpp>

using namespace cubos::engine;

namespace cubos::engine::plugins
{
    /// Plugin to handle environment settings, provided via command line.
    /// @param cubos CUBOS. main class
    void envSettingsPlugin(Cubos& cubos);
}; // namespace cubos::engine::plugins

#endif // CUBOS_ENGINE_PLUGINS_ENV_SETTINGS_HPP
