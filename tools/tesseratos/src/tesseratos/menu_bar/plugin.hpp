/// @dir
/// @brief @ref tesseratos-menu-bar-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-menu-bar-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-menu-bar-plugin Menu bar
    /// @ingroup tesseratos
    /// @brief Adds a menu bar to tesseratos.

    /// @brief Layout state.
    struct LayoutState
    {
        CUBOS_REFLECT;

        bool isReady{false};
        std::string layout;
    };

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-menu-bar-plugin
    void menuBarPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
