/// @dir
/// @brief Input plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup input-plugin

#pragma once

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/input/input.hpp>

namespace cubos::engine
{
    /// @defgroup input-plugin Input
    /// @ingroup plugins
    /// @brief Adds input handling to CUBOS.
    ///
    /// ## Bridges
    /// - @ref JSONBridge - registered with the `.bind` extension, loads @ref InputBindings assets.
    ///
    /// ## Events
    /// - @ref InputEvent - (TODO) emitted when an input event occurs.
    ///
    /// ## Resources
    /// - @ref Input - stateful input manager, used to query the input state.
    ///
    /// ## Tags
    /// - `cubos.input.update` - updates the input state.
    ///
    /// ## Dependencies
    /// - @ref assets-plugin
    /// - @ref window-plugin

    /// @brief Plugin entry function.
    /// @param cubos CUBOS. main class.
    /// @ingroup input-plugin
    void inputPlugin(Cubos& cubos);
} // namespace cubos::engine
