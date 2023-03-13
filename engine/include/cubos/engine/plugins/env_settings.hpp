#ifndef CUBOS_ENGINE_PLUGINS_ENV_SETTINGS_HPP
#define CUBOS_ENGINE_PLUGINS_ENV_SETTINGS_HPP

#include <cubos/engine/cubos.hpp>

namespace cubos::engine::plugins
{
    /// Plugin to load environment settings, provided via command line.
    /// Any duplicated setting will be overwritten.
    ///
    /// @details This plugin needs the Arguments resource to be defined. The resouce is defined
    /// when the command line arguments (argc and argv) are provided to the Cubos constructor.
    ///
    /// Startup tags:
    /// - `cubos.settings`: the settings are loaded with this tag.
    /// - `cubos.settings.env`: the settings are loaded with this tag.
    ///
    /// @param cubos CUBOS. main class
    void envSettingsPlugin(Cubos& cubos);
} // namespace cubos::engine::plugins

#endif // CUBOS_ENGINE_PLUGINS_ENV_SETTINGS_HPP
