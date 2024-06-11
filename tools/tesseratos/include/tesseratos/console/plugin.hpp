/// @dir
/// @brief @ref tesseratos-console-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-console-plugin

#pragma once

#include <cubos/engine/assets/plugin.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-console-plugin Console/Logs
    /// @ingroup tesseratos
    /// @brief Displays Cubos Logs.
    ///
    /// ## Dependencies
    /// - @ref imgui-plugin
    /// - @ref log-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup tesseratos-console-plugin
    void consolePlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos