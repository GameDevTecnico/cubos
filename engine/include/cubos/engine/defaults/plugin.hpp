/// @dir
/// @brief @ref defaults-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup defaults-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup defaults-plugin Defaults
    /// @ingroup engine
    /// @brief Adds a series of pre-configured essential plugins to the engine.

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup defaults-plugin
    void defaultsPlugin(Cubos& cubos);
} // namespace cubos::engine
