/// @dir
/// @brief @ref splitscreen-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup splitscreen-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup splitscreen-plugin Splitscreen
    /// @ingroup engine
    /// @brief Adds viewport to all active cameras to achieve a splitscreen layout.
    ///
    /// ## Dependencies
    /// - @ref renderer-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup splitscreen-plugin
    void splitscreenPlugin(Cubos& cubos);
} // namespace cubos::engine
