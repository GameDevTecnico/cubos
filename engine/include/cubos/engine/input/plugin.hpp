/// @dir
/// @brief @ref input-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup input-plugin

#pragma once

#include <cubos/engine/input/input.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup input-plugin Input
    /// @ingroup engine
    /// @brief Adds input handling to @b Cubos
    /// @see Take a look at the @ref examples-engine-input example for a demonstration of this
    /// plugin.
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
    /// ## Dependencies
    /// - @ref assets-plugin
    /// - @ref window-plugin

    /// @brief Updates the input state.
    CUBOS_ENGINE_API extern Tag inputUpdateTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup input-plugin
    CUBOS_ENGINE_API void inputPlugin(Cubos& cubos);
} // namespace cubos::engine
