/// @dir
/// @brief @ref entity-inspector-tool-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup entity-inspector-tool-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup entity-inspector-tool-plugin Entity Inspector
    /// @ingroup tool-plugins
    /// @brief Allows inspecting and modifying the components of the current @ref Selection.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup entity-inspector-tool-plugin
    void entityInspectorPlugin(Cubos& cubos);
} // namespace cubos::engine
