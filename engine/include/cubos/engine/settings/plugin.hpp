/// @dir
/// @brief @ref settings-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup settings-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    /// @defgroup settings-plugin Settings
    /// @ingroup engine
    /// @brief Adds and manages settings.
    ///
    /// Initially, if the @ref Arguments resource is present, settings will be loaded from those
    /// arguments. Then, the file at `settings.path` is loaded, as a JSON file. If the file does
    /// not exist, it is created. If it can't be parsed, the plugin aborts. Previously set settings
    /// will be overriden, and file settings will be overriden by command line arguments.
    ///
    /// ## Settings
    /// - `settings.path` - path of the settings file (default: `./settings.json`).
    ///
    /// ## Resources
    /// - @ref Settings - holds the settings.
    ///
    /// ## Startup tags
    /// - `cubos.settings` - the settings are loaded, overriding values set previously.

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup settings-plugin
    void settingsPlugin(Cubos& cubos);
} // namespace cubos::engine
