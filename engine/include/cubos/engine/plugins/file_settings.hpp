#ifndef CUBOS_ENGINE_PLUGINS_FILE_SETTINGS_HPP
#define CUBOS_ENGINE_PLUGINS_FILE_SETTINGS_HPP

#include <cubos/engine/cubos.hpp>

namespace cubos::engine::plugins
{
    /// Plugin which loads
    ///
    /// Startup Stages:
    /// - readSettings: fills the settings with the options loaded from a file.
    /// @param cubos CUBOS. main class
    void fileSettingsPlugin(Cubos& cubos);
}; // namespace cubos::engine::plugins

#endif // CUBOS_ENGINE_PLUGINS_FILE_SETTINGS_HPP
