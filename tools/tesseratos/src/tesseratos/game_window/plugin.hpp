/// @dir
/// @brief @ref tesseratos-game-window-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-game-window-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-game-window-plugin Game Window
    /// @ingroup tesseratos
    /// @brief Injects a fake window plugin into the @ref Game, which shows as a window in the editor.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-game-plugin
    void gameWindowPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
