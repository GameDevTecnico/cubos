/// @dir
/// @brief @ref tool-plugins module.

/// @file
/// @brief Plugin entry point.
/// @ingroup tool-plugins

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup tool-plugins Tools
    /// @ingroup engine
    /// @brief Adds various debugging tool plugins.
    ///
    /// Most of these tools provide their own ImGui windows which can be toggled with the @ref toolbox-plugin.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tool-plugins
    void toolsPlugin(Cubos& cubos);
} // namespace cubos::engine
