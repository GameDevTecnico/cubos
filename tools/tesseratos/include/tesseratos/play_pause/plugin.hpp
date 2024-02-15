/// @dir
/// @brief @ref tesseratos-play-pause-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-play-pause-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-play-pause-plugin Play Pause
    /// @ingroup tesseratos
    /// @brief Allows changing the current simulation speed, or even pause it.
    ///
    /// ## Dependencies
    /// - @ref imgui-plugin
    /// - @ref tesseratos-toolbox-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup tesseratos-play-pause-plugin
    void playPausePlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
