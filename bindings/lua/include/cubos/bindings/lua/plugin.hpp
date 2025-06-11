/// @dir
/// @brief @ref lua-bindings-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup lua-bindings-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::bindings::lua
{
    /// @defgroup lua-bindings-plugin Lua bindings
    /// @ingroup bindings
    /// @brief Adds lua bindings to @b Cubos
    ///
    /// ## Dependencies
    /// - @ref settings-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup lua-bindings-plugin
    void luaBindingsPlugin(cubos::engine::Cubos& cubos);

} // namespace cubos::bindings::lua
