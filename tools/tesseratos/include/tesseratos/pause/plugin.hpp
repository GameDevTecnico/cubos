/// @dir
/// @brief @ref tesseratos-pause-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-pause-plugin

#pragma once

#include <cubos/engine/cubos.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-pause-plugin Pause
    /// @ingroup tesseratos
    /// @brief Adds a play/pause button.
    ///
    /// ## Tags
    /// - `tesseratos.pause.ignore` - Systems with tag run even if the pause is enabled.
    ///
    /// ## Dependencies
    /// - @ref imgui-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup tesseratos-pause-plugin
    void pausePlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
