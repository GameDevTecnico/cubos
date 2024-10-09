/// @dir
/// @brief @ref console-tool-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup console-tool-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup console-tool-plugin Console
    /// @ingroup tool-plugins
    /// @brief Displays the log messages in an ImGui window.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup console-tool-plugin
    CUBOS_ENGINE_API void consolePlugin(Cubos& cubos);
} // namespace cubos::engine