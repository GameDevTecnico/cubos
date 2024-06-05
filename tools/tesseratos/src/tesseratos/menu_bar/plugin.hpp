/// @dir
/// @brief @ref tesseratos-menu-bar-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-menu-bar-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

#include "item.hpp"
#include "selected.hpp"

namespace tesseratos
{
    /// @defgroup tesseratos-menu-bar-plugin Menu Bar
    /// @ingroup tesseratos
    /// @brief Adds the @ref MenuBarItem and @ref MenuBarSelected components.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-game-plugin
    void menuBarPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
