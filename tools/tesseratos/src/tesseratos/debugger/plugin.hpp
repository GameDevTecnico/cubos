/// @dir
/// @brief @ref tesseratos-debugger-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-debugger-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

#include "debugger.hpp"

namespace tesseratos
{
    /// @defgroup tesseratos-debugger-plugin Debugger
    /// @ingroup tesseratos
    /// @brief Adds a resource used to manage a connection to a Cubos debugger.
    ///
    /// ## Resources
    /// - @ref Debugger - contains connection information.

    /// @brief Tool state.
    struct Debugger
    {
        CUBOS_REFLECT;

        bool isOpen{false};
    };

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-debugger-plugin
    void debuggerPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
