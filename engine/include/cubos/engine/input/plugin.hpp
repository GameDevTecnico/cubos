/// @file
/// @brief Input plugin entry point.
#pragma once

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/input/input.hpp>

namespace cubos::engine
{
    /// Plugin which adds input handling to CUBOS.
    ///
    /// @details
    ///
    /// Bridges:
    /// - `JSONBridge<InputBindings>`: registered with the `.bind` extension.
    ///
    /// Events:
    /// - `InputEvent`: emitted when an input event occurs. (TODO)
    ///
    /// Resources:
    /// - `Input`: stateful input manager, used to query the input state.
    ///
    /// Dependencies:
    /// - `assetsPlugin`
    /// - `windowPlugin`
    ///
    /// @param cubos CUBOS. main class.
    void inputPlugin(Cubos& cubos);
} // namespace cubos::engine
