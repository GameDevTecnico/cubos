/// @dir
/// @brief File settings plugin directory.

/// @file
/// @brief Plugin entry point.

#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    /// @defgroup file-settings-plugin File settings
    /// @ingroup plugins
    /// @brief Loads settings from a file.
    ///
    /// The settings file must be a JSON file, located at the path specified by the `settings.path`
    /// setting. If the setting is not defined, its assumed to be located at `./settings.json`. If
    /// the file does not exist, the plugin will abort. Any previously set setting will be
    /// overwritten if its set on the file.
    ///
    /// ## Startup tags
    /// - `cubos.settings`: the settings are loaded.
    /// - `cubos.settings.file`: the settings are loaded.

    /// @brief Plugin entry function.
    /// @param cubos CUBOS. main class
    /// @ingroup file-settings-plugin
    void fileSettingsPlugin(Cubos& cubos);
} // namespace cubos::engine
