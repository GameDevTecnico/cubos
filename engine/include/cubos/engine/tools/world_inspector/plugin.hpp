/// @dir
/// @brief @ref world-inspector-tool-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup world-inspector-tool-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup world-inspector-tool-plugin World Inspector
    /// @ingroup tool-plugins
    /// @brief Shows all of the entities in the world, and allows selecting them.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup world-inspector-tool-plugin
    CUBOS_ENGINE_API void worldInspectorPlugin(Cubos& cubos);
} // namespace cubos::engine
