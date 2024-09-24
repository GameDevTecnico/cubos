/// @dir
/// @brief @ref play-pause-tool-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup play-pause-tool-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup play-pause-tool-plugin Play Pause
    /// @ingroup tool-plugins
    /// @brief Allows changing the current simulation speed, or even pause it.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup play-pause-tool-plugin
    void playPauseToolPlugin(Cubos& cubos);
} // namespace cubos::engine
