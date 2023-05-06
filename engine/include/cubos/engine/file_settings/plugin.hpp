#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    /// Plugin to load settings, provided via a file.
    /// Any duplicated setting will be overwritten.
    ///
    /// @details The settings file must be a JSON file, located at the path specified by the
    /// `settings.path` setting. If the setting is not defined, its assumed to be located at
    /// `./settings.json`. If the file does not exist, the plugin will abort.
    ///
    /// Startup tags:
    /// - `cubos.settings`: the settings are loaded with this tag.
    /// - `cubos.settings.file`: the settings are loaded with this tag.
    ///
    /// @param cubos CUBOS. main class
    void fileSettingsPlugin(Cubos& cubos);
}; // namespace cubos::engine
