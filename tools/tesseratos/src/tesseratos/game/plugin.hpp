/// @dir
/// @brief @ref tesseratos-game-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-game-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

#include "game.hpp"

namespace tesseratos
{
    /// @defgroup tesseratos-game-plugin Game
    /// @ingroup tesseratos
    /// @brief Adds the @ref Game resource to the editor.

    /// @brief Game instance is updated, if running.
    extern cubos::engine::Tag gameUpdateTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-game-plugin
    void gamePlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
