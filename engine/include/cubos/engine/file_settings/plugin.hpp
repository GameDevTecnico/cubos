/// @dir
/// @brief @ref file-settings-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup file-settings-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    /// @defgroup file-settings-plugin File settings
    /// @ingroup engine
    /// @brief Loads settings from a file.
    ///
    /// The settings file must be a JSON file. If the file does not exist or can't be parsed, the
    /// plugin aborts. Any previously set setting will be overwritten if its set on the file.
    ///
    /// ## Settings
    /// - `settings.path` - path of the settings file (default: `./settings.json`).
    ///
    /// ## Startup tags
    /// - `cubos.settings` - the settings are loaded.
    /// - `cubos.settings.file` - the settings are loaded.

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup file-settings-plugin
    void fileSettingsPlugin(Cubos& cubos);
} // namespace cubos::engine
