/// @dir
/// @brief Environment settings plugin directory.

/// @file
/// @brief Plugin entry point.

#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    /// @defgroup env-settings-plugin Environment settings
    /// @ingroup plugins
    /// @brief Loads settings from the command line arguments.
    ///
    /// This plugin needs the Arguments resource, and thus, the constructor @ref Cubos::Cubos(int, char**) must be
    /// used to construct the application. Any previous setting will be overwritten if its set on the arguments.
    ///
    /// ## Startup tags
    /// - `cubos.settings`: the settings are loaded.
    /// - `cubos.settings.env`: the settings are loaded.

    /// @brief Plugin entry function.
    /// @param cubos CUBOS. main class
    /// @ingroup env-settings-plugin
    void envSettingsPlugin(Cubos& cubos);
} // namespace cubos::engine
