#ifndef CUBOS_ENGINE_PLUGINS_FILESETTINGS_HPP
#define CUBOS_ENGINE_PLUGINS_FILESETTINGS_HPP

#include <cubos/core/data/json_deserializer.hpp>

#include <cubos/engine/cubos.hpp>

namespace cubos::engine::plugins
{
    /// Plugin to load settings from a file. 
    ///
    /// Startup Stages:
    /// - readSettings: Deserialize settings.
    /// @param cubos CUBOS. main class
    void fileSettingsPlugin(Cubos& cubos);
}; // namespace cubos::engine::plugins

#endif // CUBOS_ENGINE_PLUGINS_FILESETTINGS_HPP
