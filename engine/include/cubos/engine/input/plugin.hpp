/// @file
#pragma once

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/input/input.hpp>

namespace cubos::engine
{
    /// Plugin which adds input handling to CUBOS.
    ///
    /// @details
    ///
    /// Events:
    /// - `InputEvent`: emitted when an input event occurs.
    ///
    /// Resources:
    /// - `Input`: stateful input manager, used to query the input state.
    ///
    /// Dependencies:
    /// - `assetsPlugin`
    ///
    /// @param cubos CUBOS. main class.
    void inputPlugin(Cubos& cubos);
} // namespace cubos::engine