/// @dir
/// @brief @ref render-defaults-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-defaults-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-defaults-plugin Defaults
    /// @ingroup engine
    /// @brief Adds the most frequently used rendering plugins to the engine.

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup render-defaults-plugin
    void renderDefaultsPlugin(Cubos& cubos);
} // namespace cubos::engine
